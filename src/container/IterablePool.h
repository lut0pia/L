#pragma once

#include "Array.h"
#include "Pool.h"

namespace L {
  template<class T>
  class IterablePool {
  protected:
    Array<T*> _objects;
    Pool<T> _pool;

  public:
    T* allocate() {
      T* object(_pool.allocate());
      _objects.push(object);
      return object;
    }
    void deallocate(T* p) {
      _pool.deallocate(p);
      if(const T* const * o = _objects.find(p)) {
        _objects.erase_fast(o - (const T* const *)_objects.begin());
      }
    }
    const Array<T*>& objects() const { return _objects; }
  };
}
