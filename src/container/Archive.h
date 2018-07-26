#pragma once

#include "../parallelism/Lock.h"
#include "../text/String.h"
#include "../time/Date.h"
#include "../hash.h"

namespace L {
  class Archive {
  public:
    struct Entry {
      Date date;
      uint32_t hash;
      uint32_t position;
      uint32_t size;
      inline operator bool() const { return size>0; }
    };
    static const size_t table_size_bytes = 1<<16; // 64KB
    static const size_t table_entry_count = table_size_bytes/sizeof(Entry);
  protected:
    String _path;
    Entry* _entries;
    Lock _lock;

  public:
    Archive(const char* path);
    ~Archive();
    inline Entry& find(const char* key) { return find(hash(key)); }
    Entry& find(uint32_t key_hash);
    void store(const char* key, const void* data, size_t size);
    void load(Entry, void*);
  };
}
