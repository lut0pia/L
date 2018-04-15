#pragma once

#include "../types.h"

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
    void change_thread_mask(uint32_t mask);
  };
}
