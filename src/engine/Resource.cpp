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
#include "../system/FileWatch.h"

using namespace L;

static Archive archive("data.bin");
static Lock lock;
static Pool<ResourceSlot> _pool;
static Array<ResourceSlot*> _slots;
static Table<Symbol, ResourceSlot*> _table;
#if !L_RLS
static Archive archive_dev("dev.bin");
static Table<String, Array<ResourceSlot*>> _source_file_resources;
#endif
static Date program_mtime = 0;

ResourceSlot::ResourceSlot(const Symbol& type, const char* url)
  : type(type), id(url), path(url, min<size_t>(strlen(url), strchr(url, '?') - url)) {
  verbose("Creating resource slot of type %s for url %s", type, url);
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
  ResourceState expected_state = ResourceState::Unloaded;
  if(state.compare_exchange_strong(expected_state, ResourceState::Loading)) {
    const uint32_t thread_mask = TaskSystem::thread_count() > 1 ? uint32_t(-2) : uint32_t(-1);
    TaskSystem::push([](void* p) {
      ResourceSlot& slot(*(ResourceSlot*)p);
      L_SCOPE_MARKERF("Resource load (%s)", (const char*)slot.id);
      slot.load_function(slot);
    }, this, thread_mask, TaskSystem::NoParent);
  }
}
bool ResourceSlot::flush() {
  if(state != ResourceState::Loaded) {
    L_SCOPE_MARKERF("Resource flush (%s)", (const char*)id);
    load();
    TaskSystem::yield_until([](void* data) {
      return ((ResourceSlot*)data)->state == ResourceState::Loaded;
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
  archive.store(typed_id, data, size);
}

#if !L_RLS
void ResourceSlot::read(Stream& stream) {
  stream >= source_files >= mtime;
}
void ResourceSlot::write(Stream& stream) const {
  stream <= source_files <= mtime;
}
Buffer ResourceSlot::read_archive_dev() {
  L_SCOPE_MARKER("Resource read dev archive");
  const Symbol typed_id = make_typed_id(type, id);
  if(Archive::Entry entry = archive_dev.find(typed_id)) {
    Buffer buffer(entry.size);
    archive_dev.load(entry, buffer);
    return buffer;
  }
  return Buffer();
}
void ResourceSlot::write_archive_dev(const void* data, size_t size) {
  L_SCOPE_MARKER("Resource write dev archive");
  const Symbol typed_id = make_typed_id(type, id);
  archive_dev.store(typed_id, data, size);
}
void ResourceSlot::update_source_file_table() {
  L_SCOPED_LOCK(lock);
  for(const String& source_file : source_files) {
    if(_source_file_resources[source_file].find(this) == nullptr) {
      _source_file_resources[source_file].push(this);
    }
  }
}
bool ResourceSlot::is_out_of_date() const {
  if(mtime < program_mtime) {
    return true;
  }
  for(const String& source_file : source_files) {
    Date file_mtime;
    if(File::mtime(source_file, file_mtime) && mtime < file_mtime) {
      return true;
    }
  }
  return false;
}
#endif

Symbol ResourceSlot::make_typed_id(const Symbol& type, const char* url) {
  return Symbol(type + ":" + url);
}
ResourceSlot* ResourceSlot::find(const Symbol& type, const char* url) {
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
void ResourceSlot::set_program_mtime(Date mtime) {
  program_mtime = mtime;
}
#if !L_RLS
void ResourceSlot::update() {
  L_SCOPE_MARKER("Resource update");
  L_SCOPED_LOCK(lock);
  static FileWatch file_watch(".");
  file_watch.update();
  for(const String& changed_file : file_watch.get_changes()) {
    if(const Array<ResourceSlot*>* resources = _source_file_resources.find(changed_file)) {
      for(ResourceSlot* slot : *resources) {
        // No reload if it hasn't been loaded in the first place
        if(slot->state == ResourceState::Loaded || slot->state == ResourceState::Failed) {
          slot->state = ResourceState::Unloaded;
        }
      }
    }
  }
  file_watch.clear();
}
Array<ResourceSlot*> ResourceSlot::slots() {
  L_SCOPED_LOCK(lock);
  return _slots;
}
#endif
