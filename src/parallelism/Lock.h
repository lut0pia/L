#pragma once

#include <atomic>

#include "TaskSystem.h"

#define L_SCOPED_LOCK(lock) L::ScopedLock LOCK__##__LINE__(lock);

namespace L {
  class Lock {
  protected:
    std::atomic<bool> _locked;

  public:
    constexpr Lock(bool locked = false)
      : _locked(locked) {}
    inline bool try_lock() {
      bool expected = false;
      return _locked.compare_exchange_strong(expected, true);
    }
    inline void lock() {
      TaskSystem::yield_until([](void* data) { return ((Lock*)data)->try_lock(); }, this);
    }
    inline void unlock() { _locked = 0; }
  };

  class ScopedLock {
  protected:
    Lock& _lock;

  public:
    inline ScopedLock(Lock& lock)
      : _lock(lock) { _lock.lock(); }
    inline ~ScopedLock() { _lock.unlock(); }
  };
}
