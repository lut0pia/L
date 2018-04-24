#pragma once

#include "../system/intrinsics.h"
#include "TaskSystem.h"

#define L_SCOPED_LOCK(lock) L::ScopedLock LOCK__ ## __LINE__(lock);

namespace L {
  class Lock {
  protected:
    uint32_t _locked;
  public:
    constexpr Lock(bool locked = false) : _locked(locked ? 1 : 0) {}
    inline bool try_lock() { return !_locked && !cas(&_locked, 0, 1); }
    inline void lock() { while(!try_lock()) TaskSystem::yield(); }
    inline void unlock() { _locked = 0; }
  };

  class ScopedLock {
  protected:
    Lock& _lock;
  public:
    inline ScopedLock(Lock& lock) : _lock(lock) { _lock.lock(); }
    inline ~ScopedLock() { _lock.unlock(); }
  };
}
