#pragma once

#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include "../objects.h"
#include "../streams/Stream.h"

namespace L {
  template <class T>
  class Array {
  private:
    T* _data;
    size_t _size,_capacity;

    inline void shift(size_t i,int offset) {
      memmove(_data+i+offset,_data+i,(_size-i)*sizeof(T));
    }

  public:
    inline Array() : _data(nullptr),_size(0),_capacity(0) {}
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
        _data = (T*)malloc(_capacity*sizeof(T));
        copy(_data,other._data,_size);
      } else _data = nullptr;
    }
    inline Array(Array&& other) : _data(other._data),_size(other._size),_capacity(other._capacity){
      other._data = nullptr;
    }
    inline ~Array() {
      if(_data){
        for(uintptr_t i(0); i<_size; i++)
          (_data+i)->~T();
        free(_data);
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
    inline T& operator[](size_t i) { return _data[i]; }
    inline const T& operator[](size_t i) const { return _data[i]; }
    inline T& front() { return operator[](0); }
    inline const T& front() const { return operator[](0); }
    inline T& back() { return operator[](_size-1); }
    inline const T& back() const { return operator[](_size-1); }
    inline T* begin() { return _data; }
    inline T* end() { return _data+_size; }
    inline const T* begin() const{ return _data; }
    inline const T* end() const{ return _data+_size; }
    template <typename... Args> inline void push(Args&&... args) { insert(_size,args...); }
    template <typename... Args> inline void pushFront(Args&&... args) { insert(0,args...); }
    inline void pop() { erase(_size-1); }

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
        if(n<_size) size(n); // Have to resize because capacity cannot be below size
        _data = (T*)realloc(_data,n*sizeof(T));
        _capacity = n;
      }
    }
    void growTo(size_t size) {
      if(size>capacity()) {
        // Avoid too low start (8 bytes min) and target power of two (byte-wise)
        size_t tmp((_capacity) ? _capacity : (max<size_t>(8u,upperpow2(sizeof(T)))/sizeof(T)));
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
    inline void insertArray(size_t i,const Array& a,int alen = -1,size_t ai = 0) { replaceArray(i,0,a,alen,ai); }
    template <typename... Args> inline void replace(size_t i,Args&&... args) { reconstruct(operator[](i),args...); }
    void replaceArray(size_t i,int len,const Array& a,int alen = -1,size_t ai = 0) {
      if(alen==-1) alen = a.size();
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
    void erase(size_t i,size_t count) {
      for(uintptr_t j(0); j<count; j++)
        (_data+i+j)->~T(); // Destruct values
      shift(i+count,-count); // Move right part
      _size -= count; // Decrease size
    }
  };

  template <class T>
  Stream& operator<<(Stream& s,const Array<T>& v) {
    s << '[';
    bool first(true);
    for(auto&& e : v){
      if(first) first = false;
      else s << ',';
      s << e;
    }
    s << ']';
    return s;
  }
}
