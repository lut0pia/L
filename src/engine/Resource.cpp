#include "Resource.h"

#include "../container/Archive.h"
#include "../container/Array.h"
#include "../container/Buffer.h"
#include "../container/Pool.h"
#include "../container/Table.h"
#include "../dev/profiling.h"
#include "../parallelism/Lock.h"
#include "../stream/CFileStream.h"
#include "../system/File.h"

using namespace L;

static Archive archive("data.bin");
static Pool<ResourceSlot> _pool;
static Array<ResourceSlot*> _slots;
static Table<Symbol, ResourceSlot*> _table;

ResourceSlot::ResourceSlot(const char* url) : id(url),
path(url, min<size_t>(strlen(url), strchr(url, '?') - url)),
persistent(false), state(Unloaded), value(nullptr) {
  // Extract file extension from path
  const char* dot = strrchr(path, '.');
  ext = dot ? dot + 1 : (const char*)path;

  // Extract parameters from url
  const char* pair = strchr(id, '?');
  while(pair && *pair) {
    pair += 1;
    const char* equal = strchr(pair, '=');
    if(!equal) break;

    const char* val = equal + 1;
    const uintptr_t val_len = strcspn(val, "&\0");
    parameters[Symbol(pair, equal - pair)] = Symbol(val, val_len);

    pair = val + val_len;
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
  if(state==ResourceSlot::Unloaded && cas((uint32_t*)&state, ResourceSlot::Unloaded, ResourceSlot::Loading)==ResourceSlot::Unloaded) {
    const uint32_t thread_mask = TaskSystem::thread_count() > 1 ? uint32_t(-2) : uint32_t(-1);
    TaskSystem::push([](void* p) {
      ResourceSlot& slot(*(ResourceSlot*)p);
      L_SCOPE_MARKERF("Resource load (%s)", (const char*)slot.id);
      slot.load_function(slot);
      slot.mtime = Date::now();
      slot.state = ResourceSlot::Loaded;
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
  // Try to find the source file in the archive
  if(Archive::Entry entry = archive.find("file:"+String(id))) {
    Date file_mtime;
    if(!File::mtime(path, file_mtime) || file_mtime<entry.date) {
      Buffer buffer(entry.size);
      archive.load(entry, buffer);
      return buffer;
    }
  }
  // Otherwise read actual source file
  if(CFileStream stream = CFileStream(path, "rb")) {
    const size_t size(stream.size());
    Buffer wtr(size);
    stream.read(wtr, size);
    return wtr;
  } else {
    warning("Couldn't read source file: %s", (const char*)path);
  }
  return Buffer();
}
void ResourceSlot::store_source_file_to_archive() {
  if(persistent) return;
  // Try to find the source file in the archive
  const String key("file:"+String(id));
  if(Archive::Entry entry = archive.find(key)) {
    Date file_mtime;
    if(!File::mtime(path, file_mtime)) return; // Source file doesn't exist
    if(file_mtime<entry.date) return; // Source file already up-to-date in archive
  }
  // Store actual source file to archive
  if(CFileStream stream = CFileStream(path, "rb")) {
    const size_t size(stream.size());
    Buffer buffer(size);
    stream.read(buffer, size);
    archive.store(key, buffer.data(), buffer.size());
  }
}
Buffer ResourceSlot::read_archive() {
  L_SCOPE_MARKER("Resource read archive");
  if(Archive::Entry entry = archive.find(id)) {
    Date file_mtime;
    if(!File::mtime(path, file_mtime) || file_mtime<entry.date) {
      Buffer buffer(entry.size);
      archive.load(entry, buffer);
      return buffer;
    }
  }
  return Buffer();
}
void ResourceSlot::write_archive(const void* data, size_t size) {
  L_SCOPE_MARKER("Resource write archive");
  archive.store(id, data, size);
}

ResourceSlot* ResourceSlot::find(const char* url) {
  static Lock lock;
  L_SCOPED_LOCK(lock);

  const Symbol id(url);
  if(ResourceSlot** found = _table.find(id))
    return *found;
  else {
    ResourceSlot* slot(new(_pool.allocate())ResourceSlot(url));
    _table[id] = slot;
    _slots.push(slot);
    return slot;
  }
}
void ResourceSlot::update() {
  if(!_slots.empty()) {
    L_SCOPE_MARKER("Resource update");
    static uintptr_t index(0);
    ResourceSlot& slot(*_slots[index%_slots.size()]);
    if(!slot.persistent // Persistent resources don't hot reload
      && slot.state == Loaded) { // No reload if it hasn't been loaded in the first place
      Date file_mtime;
      if(File::mtime(slot.path, file_mtime) && slot.mtime<file_mtime) {
        slot.state = Unloaded;
        slot.mtime = Date::now();
      }
    }
    index++;
  }
}
