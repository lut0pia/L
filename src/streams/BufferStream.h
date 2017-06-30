#pragma once

#include "Stream.h"

namespace L {
  class BufferStream : public Stream {
  protected:
    char *_start,*_cur,*_end;
  public:
    inline BufferStream(char* start,size_t size) : _start(start),_cur(start),_end(start+size) {}
    inline BufferStream(char* start,char* end) : _start(start),_cur(start),_end(end) {}
    inline size_t write(const void* data,size_t size) override {
      size = min(size,(uintptr_t)_cur-(uintptr_t)_end);
      if(size){
        memcpy(_cur,data,size);
        _cur += size;
      }
      return size;
    }
    inline size_t read(void* data,size_t size) override {
      size = min(size,(uintptr_t)_cur-(uintptr_t)_end);
      if(size){
        memcpy(data,_cur,size);
        _cur += size;
      }
      return size;
    }
    inline char get() override { return (_cur<_end) ? *_cur++ : EOF; }
    inline void put(char c) override { if(_cur<_end) *_cur++ = c; }
    inline void unget(char c) override { if(_cur>_start) *--_cur = c; }
    inline bool end() const override { return _cur>=_end; }

    inline size_t pos() const { return _cur-_start; }
  };
}
