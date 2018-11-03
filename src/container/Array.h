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
  private:
    T* _data;
    size_t _size,_capacity;

    inline void shift(uintptr_t i,intptr_t offset) {
      memmove(_data+i+offset,_data+i,(_size-i)*sizeof(T));
    }

  public:
    constexpr Array() : _data(nullptr),_size(0),_capacity(0) {}
    inline Array(const std::initializer_list<T>& il) : _data(nullptr),_size(0),_capacity(0){
      growTo(il.size());
      for(auto && e : il)
        push(e);
    }
    inline Array(const T* a,size_t size) : _data(nullptr),_size(0),_capacity(0) {
      growTo(size);
      while(_size<size)
        push(*a++);
    }
    template <typename... Args>
    inline Array(size_t s,Args&&... args) : _data(nullptr),_size(0),_capacity(0) {
      size(s,args...);
    }
    inline Array(const Array& other) : _size(other._size),_capacity(other._capacity) {
      if(!other.empty()){
        _data = (T*)Memory::alloc(_capacity*sizeof(T));
        copy(_data,other._data,_size);
      } else _data = nullptr;
    }
    inline Array(Array&& other) : _data(other._data),_size(other._size),_capacity(other._capacity){
      other._data = nullptr;
      other._size = other._capacity = 0;
    }
    inline ~Array() {
      if(_data){
        for(uintptr_t i(0); i<_size; i++)
          (_data+i)->~T();
        Memory::free(_data,_capacity*sizeof(T));
      }
    }
    inline Array& operator=(const Array& other) {
      if(this!=&other) {
        clear();
        insertArray(0,other);
      }
      return *this;
    }
    inline Array& operator=(Array&& other){
      this->~Array();
      _data = other._data;
      _size = other._size;
      _capacity = other._capacity;
      other._data = nullptr;
      return *this;
    }
    inline Array operator+(const Array& other) { Array wtr(*this); return wtr += other; }
    inline Array& operator+=(const Array& other) { insertArray(_size,other); return *this; }

    inline size_t size() const { return _size; }
    inline size_t capacity() const { return _capacity; }
    inline bool empty() const { return size()==0; }
    inline void clear() { size(0); }
    inline T& operator[](uintptr_t i) { return _data[i]; }
    inline const T& operator[](uintptr_t i) const { return _data[i]; }
    inline T& front() { return operator[](0); }
    inline const T& front() const { return operator[](0); }
    inline T& back() { return operator[](_size-1); }
    inline const T& back() const { return operator[](_size-1); }
    inline T* begin() { return _data; }
    inline T* end() { return _data+_size; }
    inline const T* begin() const{ return _data; }
    inline const T* end() const{ return _data+_size; }
    template <typename... Args> inline void push(Args&&... args) { insert(_size,args...); }
    template <typename... Args> inline void pushMultiple(const T& v, Args&&... args) { push(v); pushMultiple(args...); }
    inline void pushMultiple() {}
    template <typename... Args> inline void pushFront(Args&&... args) { insert(0,args...); }
    inline void pop() { erase_fast(_size-1); }

    template <typename... Args>
    void size(size_t n,Args&&... args) {
      if(_capacity<n) growTo(n);
      if(_size<n)
        for(uintptr_t i(_size); i<n; i++)
          new(_data+i)T(args...);
      else
        for(uintptr_t i(n); i<_size; i++)
          (_data+i)->~T();
      _size = n;
    }
    void capacity(size_t n) {
      if(n!=capacity()) {
        L_ASSERT_MSG(n>=_size, "Cannot have capacity of array inferior to its size");
        _data = (T*)Memory::realloc(_data,_capacity*sizeof(T),n*sizeof(T));
        _capacity = n;
      }
    }
    void growTo(size_t size) {
      if(size>capacity()) {
        // Avoid too low start (16 bytes min) and target power of two (uint8_t-wise)
        size_t tmp((_capacity) ? _capacity : (max<size_t>(16u,upperpow2(sizeof(T)))/sizeof(T)));
        while(size>tmp) tmp *= 2;
        capacity(tmp);
      }
    }
    inline void shrink() { capacity(size()); }
    template <typename... Args>
    void insert(size_t i,Args&&... args) {
      growTo(_size+1); // Check capacity
      shift(i,1); // Move right part
      new(_data+i)T(args...); // Place new value
      _size++; // Increase size
    }
    inline void insertArray(size_t i,const Array& a,size_t alen = size_t(-1),size_t ai = 0) { replaceArray(i,0,a,alen,ai); }
    template <typename... Args> inline void replace(size_t i,Args&&... args) { reconstruct(operator[](i),args...); }
    void replaceArray(size_t i, size_t len, const Array& a, size_t alen = size_t(-1), size_t ai = 0) {
      if(alen==size_t(-1)) alen = a.size();
      growTo(_size+(alen-len)); // Check capacity
      shift(i+len,alen-len);
      copy(&operator[](i),&a[ai],alen);
      _size += alen-len;
    }
    void erase(size_t i) {
      _data[i].~T(); // Destruct value
      shift(i+1,-1); // Move right part
      _size--; // Decrease size
    }
    inline void erase_fast(uintptr_t i) {
      _data[i].~T(); // Destruct value
      if(i<_size-1)
        memcpy(_data+i, _data+_size-1, sizeof(T)); // Swap with last element
      _size--; // Decrease size
    }
    void erase(size_t i,size_t count) {
      for(uintptr_t j(0); j<count; j++)
        (_data+i+j)->~T(); // Destruct values
      shift(i+count,-intptr_t(count)); // Move right part
      _size -= count; // Decrease size
    }

    uintptr_t find(const T& e){
      for(uintptr_t i(0); i<_size; i++)
        if(_data[i]==e)
          return i;
      return uintptr_t(-1);
    }

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
  };
}
