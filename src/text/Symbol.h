#pragma once

#include "../container/Table.h"
#include "../macros.h"
#include "../system/Memory.h"

namespace L {
  class Symbol {
  private:
    static const size_t blobSize = 2*1024*1024;
    static Table<const char*,const char*> _symbols;
    static char *_blobNext,*_blobEnd;
    const char* _string;
  public:
    constexpr Symbol() : _string(nullptr){}
    Symbol(const char* str){
      if(const char** found = _symbols.find(str))
        _string = *found;
      else {
        L_ASSERT(strcspn(str, " \t\n\v\f\r")==strlen(str));
        const size_t length(strlen(str));
        if(((size_t)_blobEnd-(size_t)_blobNext)<=length){
          _blobNext = (char*)Memory::virtualAlloc(blobSize);
          _blobEnd = _blobNext+blobSize;
        }
        strcpy(_blobNext,str);
        _string = _blobNext;
        _symbols[_string] = _string;
        _blobNext += length+1;
      }
    }
    inline bool operator==(const Symbol& other) const { return _string==other._string; }
    inline bool operator<(const Symbol& other) const { return strcmp(_string, other._string)<0; }
    inline operator const char*() const { return _string; }
    friend inline Stream& operator<<(Stream& s,const Symbol& sym){ return s << sym._string; }
    friend inline Stream& operator<(Stream& s, const Symbol& sym) { return s << sym._string << '\n'; }
    friend inline Stream& operator>(Stream& s, Symbol& sym) { sym = s.word(); return s; }
  };
}
