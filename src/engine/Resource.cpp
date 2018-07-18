#include "Resource.h"

#include "../container/Array.h"
#include "../container/Buffer.h"
#include "../container/Pool.h"
#include "../parallelism/Lock.h"
#include "../stream/CFileStream.h"
#include "../system/File.h"

using namespace L;

static Archive archive("data");
static Pool<ResourceSlotGeneric> _pool;
static Array<ResourceSlotGeneric*> _slots;
static Table<Symbol, ResourceSlotGeneric*> _table;

Symbol ResourceSlotGeneric::parameter(const char* key) {
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
Buffer ResourceSlotGeneric::read_source_file() {
  CFileStream stream(path, "rb");
  const size_t size(stream.size());
  Buffer wtr(size);
  stream.read(wtr, size);
  return wtr;
}
Buffer ResourceSlotGeneric::read_archive() {
  if(Archive::Entry entry = archive.find(id)) {
    Buffer buffer(entry.size);
    archive.load(entry, buffer);
    return buffer;
  }
  return Buffer();
}
void ResourceSlotGeneric::write_archive(const void* data, size_t size) {
  archive.store(id, data, size);
}
ResourceSlotGeneric* ResourceSlotGeneric::find(const char* url) {
  static Lock lock;
  L_SCOPED_LOCK(lock);

  const Symbol id(url);
  if(ResourceSlotGeneric** found = _table.find(id))
    return *found;
  else {
    ResourceSlotGeneric* slot(new(_pool.allocate())ResourceSlotGeneric(url));
    _table[id] = slot;
    _slots.push(slot);
    return slot;
  }
}
void ResourceSlotGeneric::update() {
  if(!_slots.empty()) {
    L_SCOPE_MARKER("ResourceSlotGeneric::update");
    static uintptr_t index(0);
    ResourceSlotGeneric& slot(*_slots[index%_slots.size()]);
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
