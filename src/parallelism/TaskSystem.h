#pragma once

namespace L {
  namespace TaskSystem {
    enum Flags {
      None,
      MainThread = 1<<1,
      NoParent = 1<<2,
    };
    typedef void(*Func)(void*);
    void init();
    void push(Func, void* = nullptr, Flags = None);
    void yield();
    void join();
  };
}
