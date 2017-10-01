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
    static const uint32_t max_thread_count = 16;

    void init();
    uint32_t thread_count();
    void push(Func, void* = nullptr, Flags = None);
    void yield();
    void join();
  };
}
