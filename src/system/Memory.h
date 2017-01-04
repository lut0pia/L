#pragma once

#include "../types.h"

namespace L {
  class Memory {
  public:
    static void* alloc(size_t);
    static void* allocZero(size_t); // Allocates and zero-fill block
    static void* realloc(void*, size_t oldsize, size_t newsize);
    static void free(void*, size_t);

    static void* virtualAlloc(size_t);
    static void virtualFree(void*, size_t);
  };
}
