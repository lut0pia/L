#pragma once

#include "DirectStream.h"

namespace L {
  class BufferStream : public DirectStream {
  protected:
    char *_start, *_cur, *_end;
  public:
    inline BufferStream(char* start, size_t size) : _start(start), _cur(start), _end(start+size) {}
    inline BufferStream(char* start, char* end) : _start(start), _cur(start), _end(end) {}
    inline size_t write(const void* data, size_t size) override {
      size = min(size, (uintptr_t)_cur-(uintptr_t)_end);
      if(size) {
        memcpy(_cur, data, size);
        _cur += size;
      }
      return size;
    }
    inline size_t read(void* data, size_t size) override {
      size = min(size, (uintptr_t)_cur-(uintptr_t)_end);
      if(size) {
        memcpy(data, _cur, size);
        _cur += size;
      }
      return size;
    }
    inline char get() override { return (_cur<_end) ? *_cur++ : (_cur++, EOF); }
    inline void put(char c) override { if(_cur<_end) *_cur++ = c; }
    inline bool end() override { return _cur>_end; }

    inline char peek() override { return *_cur; }
    inline void seek(uintptr_t i) override { _cur = _start+i; }
    inline uintptr_t tell() override { return _cur-_start; }
    inline size_t size() override { return _end-_start; }
  };
}
