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
      const uintptr_t i = _objects.find(p);
      if(i != uintptr_t(-1))
        _objects.erase_fast(i);
    }
    const Array<T*>& objects() const { return _objects; }
  };
}
