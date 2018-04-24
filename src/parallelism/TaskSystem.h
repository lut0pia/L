#pragma once

#include <cstdint> 

namespace L {
  namespace TaskSystem {
    enum Flags {
      None,
      MainThread = 1<<1,
      NoParent = 1<<2,
      AllowExec = 1<<3,
    };
    typedef void(*Func)(void*);

    void init();
    uint32_t thread_count();
    uint32_t fiber_id();
    void push(Func, void* = nullptr, uint32_t flags = None);
    void yield();
    void join();

    uint32_t thread_mask();
    void thread_mask(uint32_t mask);
  }

#define L_SCOPE_THREAD_MASK(mask) L::ScopeThreadMask L_CONCAT(THREAD_MASK_,__LINE__)(mask)
  class ScopeThreadMask {
  protected:
    const uint32_t _original_mask;
  public:
    inline ScopeThreadMask(uint32_t mask) : _original_mask(TaskSystem::thread_mask()) { TaskSystem::thread_mask(mask); }
    inline ~ScopeThreadMask() { TaskSystem::thread_mask(_original_mask); }
  };
}
