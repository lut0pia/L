#ifndef DEF_L_Buffer
#define DEF_L_Buffer

#include "../Exception.h"
#include "../general.h"

namespace L {
  template <int n, class T>
  class Buffer {
    private:
      T _array[n];
      int _w, _r;

    public:
      Buffer() : _w(0), _r(0) {}
      int index(int i) const {
        return PMod(i,n);
      }
      bool full() const {
        return _w==index(_r-1);
      }
      bool empty() const {
        return _w==_r;
      }
      void write(const T& e) {
        if(full()) L_Error("Cannot write because queue is full.");
        _array[_w] = e;
        _w = index(_w+1);
      }
      void pop() {
        if(empty()) L_Error("Cannot pop because queue is empty.");
        _r = index(_r+1);
      }
      const T& read() const {
        if(empty()) L_Error("Cannot read because queue is empty.");
        return _array[_r];
      }
  };
}

#endif


