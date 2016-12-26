#pragma once

#include "../containers/Table.h"
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
    inline Symbol() : _string(nullptr){}
    Symbol(const char* str){
      const Table<const char*,const char*>::Slot* slot(_symbols.find(str));
      if(slot){
        L_ASSERT(strcmp(str,slot->value())==0);
        _string = slot->value();
      } else {
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
    inline bool operator<(const Symbol& other) const { return _string<other._string; }
    inline operator const char*() const { return _string; }
    friend inline Stream& operator<<(Stream& s,const Symbol& sym){ return s << sym._string; }
  };
}
