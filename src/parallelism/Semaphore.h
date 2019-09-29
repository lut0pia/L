#pragma once

#include <cstdint>
#include "../macros.h"

namespace L {
  class Semaphore {
    L_NOCOPY(Semaphore)
  protected:
    void* _data;
  public:
    Semaphore(uint32_t initial_count = 0);
    ~Semaphore();
    bool try_get();
    void get();
    void put();
  };
}
