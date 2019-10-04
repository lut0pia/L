#pragma once

#include <new>
#include "../stream/serial_bin.h"
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
    inline Buffer(const void* data, size_t size) : _data(Memory::alloc(size)), _size(size) {
      memcpy(_data, data, _size);
    }
    inline ~Buffer() {
      if(_data) Memory::free(_data, _size);
    }
    inline Buffer& operator=(Buffer&& other) {
      this->~Buffer();
      _data = other._data;
      _size = other._size;
      other._data = nullptr;
      other._size = 0;
      return *this;
    }
    inline void* data(uintptr_t offset = 0) { return ((uint8_t*)_data) + offset; }
    inline const void* data(uintptr_t offset = 0) const { return ((uint8_t*)_data) + offset; }
    inline void size(size_t new_size) { _data = Memory::realloc(_data, _size, new_size); _size = new_size; }
    inline size_t size() const { return _size; }
    inline operator void*() const { return _data; }

    friend inline Stream& operator<=(Stream& s, const Buffer& v) { s <= v._size; s.write(v._data, v._size); return s; }
    friend inline Stream& operator>=(Stream& s, Buffer& v) { size_t size; s >= size; v.~Buffer(); new(&v)Buffer(size); s.read(v._data, size); return s; }
  };
}
