#pragma once

#include "../system/Memory.h"

namespace L {
  class Buffer {
  protected:
    void* _data;
    size_t _size;
  public:
    constexpr Buffer() : _data(nullptr), _size(0) {}
    inline Buffer(Buffer&& other) : _data(other._data), _size(other._size) {
      other._data = nullptr;
      other._size = 0;
    }
    inline Buffer(size_t size) : _data(Memory::alloc(size)), _size(size) {}
    inline ~Buffer() {
      if(_data) Memory::free(_data, _size);
    }
    inline void* data() { return _data; }
    inline const void* data() const { return _data; }
    inline void size(size_t new_size) { _data = Memory::realloc(_data, _size, new_size); _size = new_size; }
    inline size_t size() const { return _size; }
    inline operator void*() const { return _data; }
  };
}
