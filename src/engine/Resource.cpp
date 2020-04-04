#include "Resource.h"

#include <cstdlib>

#include "../container/Archive.h"
#include "../container/Array.h"
#include "../container/Buffer.h"
#include "../container/Pool.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../parallelism/Lock.h"
#include "../stream/BufferStream.h"
#include "../stream/CFileStream.h"
#include "../stream/StringStream.h"
#include "../system/File.h"

using namespace L;

static Archive archive("data.bin");
#if !L_RLS
static Archive archive_dev("dev.bin");
#endif
static Pool<ResourceSlot> _pool;
static Array<ResourceSlot*> _slots;
static Table<Symbol, ResourceSlot*> _table;

ResourceSlot::ResourceSlot(const Symbol& type, const char* url)
  : type(type), id(url), path(url, min<size_t>(strlen(url), strchr(url, '?') - url)) {
  // Extract file extension from path
  const char* dot = strrchr(path, '.');
  ext = dot ? dot + 1 : (const char*)path;

  // Extract parameters from url
  const char* pair = strchr(url, '?');
  while(pair && *pair) {
    pair += 1;
    const char* equal = strchr(pair, '=');
    const char* amp = strchr(pair, '&');
    if(equal && (!amp || equal < amp)) {
      const char* val = equal + 1;
      const uintptr_t val_len = strcspn(val, "&\0");
      parameters[Symbol(pair, equal - pair)] = Symbol(val, val_len);
      pair = val + val_len;
    } else if(amp) {
      parameters[Symbol(pair, amp - pair)] = Symbol("");
      pair = amp;
    } else {
      parameters[Symbol(pair)] = Symbol("");
      break;
    }
  }
}

Symbol ResourceSlot::parameter(const Symbol& key) const {
  return parameters.get(key, Symbol());
}
bool ResourceSlot::parameter(const Symbol& key, Symbol& param_value) const {
  if(const Symbol param = parameter(key)) {
    param_value = param;
    return true;
  }
  return false;
}
bool ResourceSlot::parameter(const Symbol& key, uint32_t& param_value) const {
  if(const Symbol param = parameter(key)) {
    param_value = atoi(param);
    return true;
  }
  return false;
}
bool ResourceSlot::parameter(const Symbol& key, float& param_value) const {
  if(const Symbol param = parameter(key)) {
    param_value = float(atof(param));
    return true;
  }
  return false;
}

void ResourceSlot::load() {
  if(state == ResourceSlot::Unloaded && cas((uint32_t*)&state, ResourceSlot::Unloaded, ResourceSlot::Loading) == ResourceSlot::Unloaded) {
    const uint32_t thread_mask = TaskSystem::thread_count() > 1 ? uint32_t(-2) : uint32_t(-1);
    TaskSystem::push([](void* p) {
      ResourceSlot& slot(*(ResourceSlot*)p);
      L_SCOPE_MARKERF("Resource load (%s)", (const char*)slot.id);
      slot.load_function(slot);
    }, this, thread_mask, TaskSystem::NoParent);
  }
}
bool ResourceSlot::flush() {
  if(state != ResourceSlot::Loaded) {
    L_SCOPE_MARKERF("Resource flush (%s)", (const char*)id);
    load();
    TaskSystem::yield_until([](void* data) {
      return ((ResourceSlot*)data)->state == ResourceSlot::Loaded;
    }, this);
  }
  return value != nullptr;
}

Buffer ResourceSlot::read_source_file() {
  // If a source buffer was provided, use it
  if(source_buffer) {
    return Buffer(source_buffer, source_buffer.size());
  }
  // Otherwise read source file from path
  if(CFileStream stream = CFileStream(path, "rb")) {
    // Add it to source files
    if(!source_files.find(String(path))) {
      source_files.push(path);
    }
    const size_t size(stream.size());
    Buffer wtr(size);
    stream.read(wtr, size);
    return wtr;
  } else {
    warning("Couldn't read source file: %s", (const char*)path);
  }
  return Buffer();
}
Buffer ResourceSlot::read_archive() {
  L_SCOPE_MARKER("Resource read archive");
  const Symbol typed_id = make_typed_id(type, id);

#if !L_RLS
  if(Archive::Entry entry = archive_dev.find(typed_id)) {
    Buffer buffer(entry.size);
    archive_dev.load(entry, buffer);
    BufferStream stream((char*)buffer.data(), buffer.size());
    stream >= source_files >= mtime;

    if(is_out_of_date()) {
      return Buffer(); // Force loading from source if entry is out of date
    }
  }
#endif

  if(Archive::Entry entry = archive.find(typed_id)) {
    Buffer buffer(entry.size);
    archive.load(entry, buffer);
    return buffer;
  }

  return Buffer();
}
void ResourceSlot::write_archive(const void* data, size_t size) {
  L_SCOPE_MARKER("Resource write archive");
  const Symbol typed_id = make_typed_id(type, id);

#if !L_RLS
  {
    StringStream stream;
    stream <= source_files <= mtime;
    archive_dev.store(typed_id, stream.string().begin(), stream.string().size());
  }
#endif

  archive.store(typed_id, data, size);
}

bool ResourceSlot::is_out_of_date() const {
  for(const String& source_file : source_files) {
    Date file_mtime;
    if(File::mtime(source_file, file_mtime) && mtime < file_mtime) {
      return true;
    }
  }
  return false;
}

Symbol ResourceSlot::make_typed_id(const Symbol& type, const char* url) {
  return Symbol(type + ":" + url);
}
ResourceSlot* ResourceSlot::find(const Symbol& type, const char* url) {
  static Lock lock;
  L_SCOPED_LOCK(lock);

  const Symbol typed_id = make_typed_id(type, url);
  if(ResourceSlot** found = _table.find(typed_id))
    return *found;
  else {
    ResourceSlot* slot(new(_pool.allocate())ResourceSlot(type, url));
    _table[typed_id] = slot;
    _slots.push(slot);
    return slot;
  }
}
void ResourceSlot::update() {
  if(!_slots.empty()) {
    L_SCOPE_MARKER("Resource update");
    static uintptr_t index = 0;
    ResourceSlot& slot(*_slots[index%_slots.size()]);
    if((slot.state == Loaded || slot.state == Failed) // No reload if it hasn't been loaded in the first place
      && slot.is_out_of_date()) {
      slot.state = Unloaded;
      slot.mtime = Date::now();
    }
    index++;
  }
}
