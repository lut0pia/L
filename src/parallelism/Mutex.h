#pragma once

#include "../macros.h"

namespace L {
  class Mutex {
    L_NOCOPY(Mutex)
  protected:
    void* _data;
  public:
    Mutex();
    ~Mutex();
    bool try_lock();
    void lock();
    void unlock();
  };

  class ScopedMutex {
  protected:
    Mutex& _mutex;
  public:
    inline ScopedMutex(Mutex& mutex) :_mutex(mutex) { _mutex.lock(); }
    inline ~ScopedMutex() { _mutex.unlock(); }
  };
}
