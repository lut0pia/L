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

Symbol ResourceSlot::parameter(const char* key) {
  const size_t key_length(strlen(key));
  const char* pair(id);
  while(true) {
    pair = strstr(pair+1, key); // Find all occurences of the key
    if(!pair) break; // Couldn't find the key
    if(pair[-1]!='?' && pair[-1]!='&') continue; // Make sure it's prefixed with ? or &
    if(pair[key_length]!='=') continue; // Make sure it's suffixed with =
    const char* value(pair+key_length+1);
    const size_t value_length(strcspn(value, "&\0"));
    return Symbol(value, value_length);
  }
  return Symbol();
}
bool ResourceSlot::parameter(const char* key, uint32_t& value) {
  if(const Symbol param = parameter(key)) {
    value = atoi(param);
    return true;
  }
  return false;
}
bool ResourceSlot::parameter(const char* key, float& value) {
  if(const Symbol param = parameter(key)) {
    value = atof(param);
    return true;
  }
  return false;
}

Buffer ResourceSlot::read_source_file() {
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
  L_SCOPE_MARKER("ResourceSlot::read_archive");
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
  archive.store(id, data, size);
}
void ResourceSlot::serialize(Stream& stream) {
  stream <= dependencies.size();
  for(const ResourceSlot* dependency : dependencies) {
    stream <= dependency->id;
  }
}
void ResourceSlot::unserialize(Stream& stream) {
  size_t dependency_count;
  stream >= dependency_count;
  while(dependency_count--) {
    Symbol dependency_id;
    stream >= dependency_id;
    dependencies.push(find(dependency_id));
  }
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
      for(const ResourceSlot* dependency : slot.dependencies) {
        if(slot.mtime<dependency->mtime) {
          slot.state = Unloaded;
        }
      }
    }
    index++;
  }
}
