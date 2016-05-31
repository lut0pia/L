#pragma once

#include "../macros.h"

namespace L {
  template <int n, class T>
  class StaticRing {
    private:
      T _array[n];
      int _w, _r;

    public:
      inline StaticRing() : _w(0), _r(0) {}
      inline int index(int i) const {return pmod(i,n);}
      inline bool full() const {return _w==index(_r-1);}
      inline bool empty() const {return _w==_r;}
      int size() const {return ((_r<=_w)?(_w-_r):(n-(_r-_w)));}
      void push(const T& e) {
        if(full()) L_ERROR("Cannot push because static ring is full.");
        _array[_w] = e;
        _w = index(_w+1);
      }
      void pop() {
        if(empty()) L_ERROR("Cannot pop because static ring is empty.");
        _r = index(_r+1);
      }
      const T& top() const {
        if(empty()) L_ERROR("Cannot read because static ring is empty.");
        return _array[_r];
      }
  };
}
