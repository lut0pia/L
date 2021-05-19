#pragma once

#include <cstring>
#include <initializer_list>
#include "../objects.h"
#include "../stream/Stream.h"
#include "../system/Memory.h"
#include "../dev/debug.h"

namespace L {
  template <class T>
  class Array {
  protected:
    T* _data = nullptr;
    size_t _size = 0, _capacity = 0;

    inline void shift(uintptr_t i, intptr_t offset) {
      memmove((void*)(_data + i + offset), _data + i, (_size - i) * sizeof(T));
    }

  public:
    constexpr Array() {}
    inline Array(const std::initializer_list<T>& il) {
      grow_to(il.size());
      for(auto&& e : il)
        push(e);
    }
    inline Array(const T* a, size_t size) {
      grow_to(size);
      while(_size < size)
        push(*a++);
    }
    template <typename... Args>
    inline Array(size_t s, Args&&... args) {
      size(s, args...);
    }
    inline Array(const Array& other) : _size(other._size), _capacity(other._capacity) {
      if(!other.empty()) {
        _data = (T*)Memory::alloc(_capacity * sizeof(T));
        copy(_data, other._data, _size);
      } else _data = nullptr;
    }
    inline Array(Array&& other) : _data(other._data), _size(other._size), _capacity(other._capacity) {
      other._data = nullptr;
      other._size = other._capacity = 0;
    }
    inline ~Array() {
      if(_data) {
        for(uintptr_t i(0); i < _size; i++)
          (_data + i)->~T();
        Memory::free(_data, _capacity * sizeof(T));
      }
    }
    inline Array& operator=(const Array& other) {
      if(this != &other) {
        clear();
        insert_array(0, other.begin(), other.size());
      }
      return *this;
    }
    inline Array& operator=(Array&& other) {
      this->~Array();
      _data = other._data;
      _size = other._size;
      _capacity = other._capacity;
      other._data = nullptr;
      return *this;
    }
    inline Array operator+(const Array& other) { Array wtr(*this); return wtr += other; }
    inline Array& operator+=(const Array& other) { push_array(other.begin(), other.size()); return *this; }

    inline size_t size() const { return _size; }
    inline size_t capacity() const { return _capacity; }
    inline bool empty() const { return size() == 0; }
    inline T& operator[](uintptr_t i) { return _data[i]; }
    inline const T& operator[](uintptr_t i) const { return _data[i]; }
    inline T& front() { return operator[](0); }
    inline const T& front() const { return operator[](0); }
    inline T& back() { return operator[](_size - 1); }
    inline const T& back() const { return operator[](_size - 1); }
    inline T* begin() { return _data; }
    inline T* end() { return _data + _size; }
    inline const T* begin() const { return _data; }
    inline const T* end() const { return _data + _size; }
    template <typename... Args> inline void push(Args&&... args) { insert(_size, args...); }
    template <typename... Args> inline void push_array(const T* a, size_t len) { insert_array(_size, a, len); }
    template <typename... Args> inline void push_multiple(const T& v, Args&&... args) { push(v); push_multiple(args...); }
    inline void push_multiple() {}
    template <typename... Args> inline void push_front(Args&&... args) { insert(0, args...); }
    inline void pop() { erase_fast(_size - 1); }

    template <typename... Args>
    void size(size_t n, Args&&... args) {
      grow_to(n);
      if(_size < n)
        for(uintptr_t i(_size); i < n; i++)
          new(_data + i)T(args...);
      else
        for(uintptr_t i(n); i < _size; i++)
          (_data + i)->~T();
      _size = n;
    }
    void clear() {
      for(uintptr_t i(0); i < _size; i++) {
        (_data + i)->~T();
      }
      _size = 0;
    }
    void capacity(size_t n) {
      if(n != capacity()) {
        L_ASSERT_MSG(n >= _size, "Cannot have capacity of array inferior to its size");
        _data = (T*)Memory::realloc(_data, _capacity * sizeof(T), n * sizeof(T));
        _capacity = n;
      }
    }
    inline void grow_to(size_t size) {
      if(size > capacity()) {
        // Avoid too low start (16 bytes min) and target power of two (uint8_t-wise)
        capacity(max<size_t>(16u, upperpow2(size * sizeof(T))) / sizeof(T));
      }
    }
    inline void shrink() { capacity(size()); }

    template <typename... Args>
    void insert(size_t i, Args&&... args) {
      grow_to(_size + 1); // Check capacity
      shift(i, 1); // Move right part
      new(_data + i)T(args...); // Place new value
      _size++; // Increase size
    }
    inline void insert_array(size_t i, const T* a, size_t len) { replace_array(i, 0, a, len); }

    template <typename... Args> inline void replace(size_t i, Args&&... args) { reconstruct(operator[](i), args...); }
    void replace_array(size_t i, size_t len, const T* a, size_t alen) {
      grow_to(_size + (alen - len)); // Check capacity
      shift(i + len, alen - len);
      copy(begin() + i, a, alen);
      _size += alen - len;
    }

    void erase(size_t i) {
      _data[i].~T(); // Destruct value
      shift(i + 1, -1); // Move right part
      _size--; // Decrease size
    }
    inline void erase_fast(uintptr_t i) {
      _data[i].~T(); // Destruct value
      if(i < _size - 1)
        memcpy((void*)(_data + i), _data + _size - 1, sizeof(T)); // Swap with last element
      _size--; // Decrease size
    }
    void erase(size_t i, size_t count) {
      for(uintptr_t j(0); j < count; j++)
        (_data + i + j)->~T(); // Destruct values
      shift(i + count, -intptr_t(count)); // Move right part
      _size -= count; // Decrease size
    }

    template <typename P>
    const T* find(P p) const {
      for(const T& e : *this) {
        if(p(e)) {
          return &e;
        }
      }
      return nullptr;
    }
    inline const T* find(const T& k) const { return find([&k](const T& e) {return e == k; }); }

    friend Stream& operator<<(Stream& s, const Array& v) {
      s << '[';
      bool first(true);
      for(auto&& e : v) {
        if(first) first = false;
        else s << ',';
        s << e;
      }
      s << ']';
      return s;
    }
    friend Stream& operator<(Stream& s, const Array& v) { for(const auto& e : v) { s << "-\n"; s < e; } return s << ".\n"; }
    friend Stream& operator>(Stream& s, Array& v) { T e; while(true) { if(strcmp(s.word(), "-")) break; s > e; v.push(e); } return s; }
    friend Stream& operator<=(Stream& s, const Array& v) { s <= v.size(); for(const T& e : v) s <= e; return s; }
    friend Stream& operator>=(Stream& s, Array& v) { auto in_size(v.size()); s >= in_size; v.size(in_size); for(T& e : v) s >= e; return s; }

    friend uint32_t hash(const Array& v) { uint32_t h = 0; for(const T& e : v) hash_combine(h, e); return h; }
  };
}
