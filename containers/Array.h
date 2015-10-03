#ifndef DEF_L_Array
#define DEF_L_Array

#include <cstdlib>
#include <cstring>
#include <functional>
#include "../Object.h"

namespace L {
  template <class T>
  class Array {
    private:
      void* _data;
      size_t _size, _capacity;

      inline void shift(size_t i, int offset) {
        memmove(&operator[](i+offset),&operator[](i),(_size-i)*sizeof(T));
      }

    public:
      Array() : _data(NULL), _size(0), _capacity(0) {}
      Array(const T* a, size_t size) : _data(NULL), _size(0), _capacity(0) {
        capacity(size);
        while(_size<size)
          push(*a++);
      }
      Array(size_t size, const T& value = T()) : _data(NULL), _size(0), _capacity(0) {
        capacity(size);
        while(_size<size)
          push(value);
      }
      Array(const Array& other) : _size(other._size), _capacity(other._size) {
        _data = malloc(_size*sizeof(T));
        Object::copy(&operator[](0),&other[0],_size);
      }
      ~Array() {
        for(size_t i(0); i<_size; i++)
          Object::destruct(operator[](i));
        free(_data);
      }
      Array& operator=(const Array& other) {
        Object::reconstruct(*this,other);
        return *this;
      }
      inline Array operator+(const Array& other) {Array wtr(*this); return wtr += other;}
      inline Array& operator+=(const Array& other) {insert(_size,other); return *this;}

      inline size_t size() const {return _size;}
      inline size_t capacity() const {return _capacity;}
      inline bool empty() const {return size()==0;}
      inline void clear() {size(0);}
      inline T& operator[](size_t i) {return *(((T*)_data)+i);}
      inline const T& operator[](size_t i) const {return *(((T*)_data)+i);}
      inline T& front() {return operator[](0);}
      inline const T& front() const {return operator[](0);}
      inline T& back() {return operator[](_size-1);}
      inline const T& back() const {return operator[](_size-1);}
      template <typename... Args> inline void push(Args&&... args) {insert(_size,args...);}
      inline void pop() {erase(_size-1);}

      void size(size_t n) {
        if(_capacity<n) capacity(n);
        while(_size<n) push();
        while(_size>n) pop();
      }
      void capacity(size_t n) {
        if(n<_size) size(n); // Have to resize because capacity cannot be below size
        _data = realloc(_data,n*sizeof(T));
        _capacity = n;
      }
      void grow() {
        capacity((_capacity)?_capacity*2:1); // Double capacity or at least make it one
      }
      void shrink() {
        capacity(size());
      }
      template <typename... Args>
      void insert(size_t i, Args&&... args) {
        if(_size+1>_capacity) grow(); // Check capacity
        shift(i,1); // Move right part
        Object::construct(operator[](i),args...); // Place new value
        _size++; // Increase size
      }
      void insert(size_t i, const Array& a) {
        while(_size+a.size()>_capacity) grow(); // Check capacity
        shift(i,a.size()); // Move right part
        Object::copy(&operator[](i),&a[0],a.size());
        _size += a.size();
      }
      void erase(size_t i) {
        Object::destruct(operator[](i)); // Destruct value
        shift(i+1,-1); // Move right part
        _size--; // Decrease size
      }
      void erase(size_t i, size_t count) {
        Object::destruct(&operator[](i),count); // Destruct values
        shift(i+count,-count); // Move right part
        _size -= count; // Decrease size
      }

      void foreach(const std::function<void(const T&)>& f) const {
        for(size_t i(0); i<_size; i++)
          f(operator[](i));
      }
      void foreach(const std::function<void(T&)>& f) {
        for(size_t i(0); i<_size; i++)
          f(operator[](i));
      }

      template <typename... Args>
      inline static Array make(Args&&... args) {
        Array wtr;
        staticPush(wtr,args...);
        return wtr;
      }
      template <typename... Args>
      inline static void staticPush(Array& a, const T& e, Args&&... args) {
        a.push(e);
        staticPush(a,args...);
      }
      inline static void staticPush(Array& a) {}
  };
}

#endif


