#ifndef DEF_L_Array
#define DEF_L_Array

#include <cstdlib>
#include <cstring>
#include <functional>
#include "../general.h"

namespace L {
  template <class T>
  class Array {
    protected:
      void* _data;
      size_t _size, _capacity;

    public:
      Array() : _data(NULL), _size(0), _capacity(0) {}
      Array(const Array& other) : _size(other._size), _capacity(other._size) {
        _data = malloc(_size*sizeof(T));
        memcpy(_data,other._data,_size*sizeof(T));
      }
      ~Array() {
        for(int i(0); i<_size; i++)
          operator[](i).~T();
        free(_data);
      }
      Array& operator=(const Array& other) {
        reconstruct(*this,other);
        return *this;
      }
      inline size_t size() const {return _size;}
      inline size_t capacity() const {return _capacity;}
      inline bool empty() const {return size()==0;}
      inline void clear() {size(0);}
      inline T& operator[](int i) {return *(((T*)_data)+i);}
      inline const T& operator[](int i) const {return *(((T*)_data)+i);}
      inline T& back() {return operator[](_size-1);}
      inline const T& back() const {return operator[](_size-1);}

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
      void push(Args&&... args) {
        if(_size+1>_capacity) grow();
        new(&operator[](_size)) T(args...);
        _size++;
      }
      void pop() {
        operator[](--_size).~T();
      }

      void foreach(const std::function<void(const T&)>& f) const {
        for(int i(0); i<_size; i++)
          f(operator[](i));
      }
  };
}

#endif


