#ifndef DEF_L_Array
#define DEF_L_Array

#include <cstdlib>
#include <cstring>
#include <functional>
#include "../general.h"

namespace L {
  template <class T>
  class Array {
    private:
      void* _data;
      size_t _size, _capacity;

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
        for(size_t i(0); i<_size; i++)
          new(&operator[](i)) T(other[i]);
      }
      ~Array() {
        for(size_t i(0); i<_size; i++)
          operator[](i).~T();
        free(_data);
      }
      Array& operator=(const Array& other) {
        reconstruct(*this,other);
        return *this;
      }
      Array operator+(const Array& other) {
        Array wtr(*this);
        return wtr += other;
      }
      Array& operator+=(const Array& other) {
        for(size_t i(0); i<other.size(); i++)
          push(other[i]);
        return *this;
      }

      inline size_t size() const {return _size;}
      inline size_t capacity() const {return _capacity;}
      inline bool empty() const {return size()==0;}
      inline void clear() {size(0);}
      inline T& operator[](size_t i) {return *(((T*)_data)+i);}
      inline const T& operator[](size_t i) const {return *(((T*)_data)+i);}
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
        if(i<_size) memmove(&operator[](i+1),&operator[](i),(_size-i)*sizeof(T)); // Move right part
        new(&operator[](i)) T(args...); // Place new value
        _size++; // Increase size
      }
      void erase(size_t i) {
        operator[](i).~T(); // Destruct value
        if(i<_size-1) memmove(&operator[](i),&operator[](i+1),(_size-i)*sizeof(T)); // Move right part
        _size--; // Decrease size
      }

      void foreach(const std::function<void(const T&)>& f) const {
        for(size_t i(0); i<_size; i++)
          f(operator[](i));
      }
      void foreach(const std::function<void(T&)>& f) {
        for(size_t i(0); i<_size; i++)
          f(operator[](i));
      }
  };
}

#endif


