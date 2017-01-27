#pragma once

#include "../macros.h"
#include "../types.h"

namespace L {
  template <int n, class T>
  class StaticStack {
  private:
    byte _array[n*sizeof(T)];
    uintptr_t _size;

  public:
    constexpr StaticStack() : _array(), _size(0) {}
    inline ~StaticStack() { while(!empty()) pop(); }
    template <class... Args>
    inline void push(Args&&... args) {
      L_ASSERT(_size<n);
      _size++;
      ::new(&top())T(args...);
    }
    inline void pop() {
      L_ASSERT(_size>0);
      top().~T();
      _size--;
    }
    inline T& top(uintptr_t i = 0) { return *((T*)_array+(_size-i-1)); }
    inline const T& top(uintptr_t i = 0) const { return *((T*)_array+(_size-i-1)); }
    inline T& bottom(uintptr_t i = 0) { return *((T*)_array+i); }
    inline const T& bottom(uintptr_t i = 0) const { return *((T*)_array+i); }
    inline T& operator[](uintptr_t i) { return top(i); }
    inline size_t size() const { return _size; }
    inline void size(size_t s) {
      while(s<size()) pop();
      while(s>size()) push();
    }
    inline bool empty() const { return size()==0; }
  };
}
