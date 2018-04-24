#pragma once

#include "../container/Raw.h"
#include "../macros.h"
#include "../types.h"

namespace L {
  class Semaphore {
    L_NOCOPY(Semaphore)
  protected:
    Raw<sizeof(void*)*4> _data;
  public:
    Semaphore(uint32_t initial_count = 0, uint32_t max_count = 1<<30);
    ~Semaphore();
    bool try_get();
    void get();
    void put();
  };
}
