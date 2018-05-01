#pragma once

#include "../container/Table.h"
#include "../dev/debug.h"
#include "../parallelism/Lock.h"
#include "../system/Memory.h"

namespace L {
  //! String interning mechanism
  //! Should be thread-safe
  class Symbol {
  private:
    static const size_t blob_size = 2*1024*1024;
    static Table<uint32_t, const char*> _symbols;
    static char *_blob_next, *_blob_end;
    const char* _string;
  public:
    constexpr Symbol() : _string(nullptr) {}
    inline Symbol(const char* str) : Symbol(str, strlen(str)) {}
    Symbol(const char* str, size_t length) {
      static Lock lock;
      L_SCOPED_LOCK(lock);
      const uint32_t hash(fnv1a(str, length));
      if(const char** found = _symbols.find(hash))
        _string = *found;
      else {
        L_ASSERT(strcspn(str, " \t\n\v\f\r")>=length);
        if(size_t(_blob_end)-size_t(_blob_next)<=(length+1)) {
          _blob_next = (char*)Memory::virtual_alloc(blob_size);
          _blob_end = _blob_next+blob_size;
        }
        memcpy(_blob_next, str, length);
        _blob_next[length] = '\0';
        _string = _blob_next;
        _symbols[hash] = _string;
        _blob_next += length+1;
      }
    }
    inline bool operator==(const Symbol& other) const { return _string==other._string; }
    inline bool operator<(const Symbol& other) const { return strcmp(_string, other._string)<0; }
    inline operator const char*() const { return _string; }
    inline explicit operator bool() const { return _string!=nullptr; }
    friend inline Stream& operator<<(Stream& s, const Symbol& sym) { return s << sym._string; }
    friend inline Stream& operator<(Stream& s, const Symbol& sym) { return s << sym._string << '\n'; }
    friend inline Stream& operator>(Stream& s, Symbol& sym) { sym = s.word(); return s; }
    friend inline uint32_t hash(const Symbol& sym) { return uint32_t(uintptr_t(sym._string)); }
  };
}
