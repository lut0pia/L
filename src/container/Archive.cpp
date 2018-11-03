#include "Archive.h"

#include "../hash.h"
#include "../stream/CFileStream.h"

using namespace L;

Archive::Archive(const char* path) : _path(path), _entries((Entry*)Memory::alloc(table_size_bytes)) {
  CFileStream read_stream(_path, "rb");
  if(read_stream)
    read_stream.read(_entries, table_size_bytes);
  else {
    CFileStream write_stream(_path, "wb");
    memset(_entries, 0, table_size_bytes);
    write_stream.write(_entries, table_size_bytes);
  }
}
Archive::~Archive() {
  Memory::free(_entries, table_size_bytes);
}
Archive::Entry& Archive::find(uint32_t key_hash) {
  uintptr_t index(key_hash%table_entry_count);
  while(index<table_entry_count) {
    Entry& e(_entries[index]);
    if(e.hash==key_hash || e.hash==0)
      return e;
    index++;
  }
  static Entry none {};
  return none;
}
void Archive::store(const char* key, const void* data, size_t size) {
  L_ASSERT(size>0);
  L_SCOPED_LOCK(_lock);
  CFileStream file_stream(_path, "rb+");
  uintptr_t key_hash(hash(key));
  Entry& e(find(key_hash));

  // Reuse space if it's enough, otherwise or if there was no entry before,
  // append to end of file
  if(!e.hash || e.size<size) {
    e.position = uint32_t(file_stream.size());
  }

  e.date = Date::now();
  e.hash = key_hash;
  e.size = uint32_t(size);
  file_stream.seek(uintptr_t(&e)-uintptr_t(_entries));
  file_stream.write(&e, sizeof(e));
  file_stream.seek(e.position);
  file_stream.write(data, size);
}
void Archive::load(Entry entry, void* dst) {
  L_ASSERT(entry.size>0);
  CFileStream file_stream(_path, "rb");
  file_stream.seek(entry.position);
  file_stream.read(dst, entry.size);
}
