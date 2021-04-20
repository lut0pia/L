#include "Memory.h"

#include <cstdlib>
#include <cstring>
#include "../dev/profiling.h"
#include "../math/math.h"

using namespace L;

#if L_USE_DEBUG_ALLOC
void* Memory::alloc(size_t size) {
  void* ptr = virtual_alloc(size + 8);
  L_ASSERT(ptr != nullptr);
  *(uintptr_t*)ptr = size;
  ptr = (uint8_t*)ptr + 8;
  return ptr;
}
void* Memory::alloc_zero(size_t size) {
  void* ptr = alloc(size);
  memset(ptr, 0, size);
  return ptr;
}
void* Memory::realloc(void* oldptr, size_t oldsize, size_t newsize) {
  void* ptr = alloc(newsize);
  if(oldptr != nullptr) {
    memcpy(ptr, oldptr, min(oldsize, newsize));
    free(oldptr, oldsize);
  }
  return ptr;
}
void Memory::free(void* ptr, size_t size) {
  L_ASSERT(ptr != nullptr);
  ptr = (uint8_t*)ptr - 8;
  L_ASSERT(*(uintptr_t*)ptr == size);
  virtual_free(ptr, size + 8);
}
#elif L_USE_MALLOC
void* Memory::alloc(size_t size) { return malloc(size); }
void* Memory::alloc_zero(size_t size) { return ::calloc(size, 1); }
void* Memory::realloc(void* ptr, size_t, size_t newsize) { return ::realloc(ptr, newsize); }
void Memory::free(void* ptr, size_t) { ::free(ptr); }
#else
const size_t max_size = 1 << 20;
static void* freelists[128] = {};
static size_t allocated(0), unused(0), wasted(0);

// Cannot allocate less than 8 bytes for alignment purposes
inline uintptr_t freelist_index(size_t size) {
  return (size<=512) ? (((size+7)/8)-1) : (clog2(size)+55);
}
inline void freelist_index_size(size_t size, uintptr_t& index, size_t& padded_size) {
  if(size<=512) {
    index = ((size+7)/8)-1;
    padded_size = (index+1)*8;
  } else {
    index = clog2(size);
    padded_size = uintptr_t(1)<<index;
    index += 55;
  }
}

void* Memory::alloc(size_t size) {
  if(size >= max_size) { // Big allocations go directly to the system
    allocated += size;
    L_COUNT_MARKER("Allocated memory", allocated);
    return virtual_alloc(size);
  }
  uintptr_t index;
  size_t padded_size;
  freelist_index_size(size, index, padded_size);

  wasted += padded_size - size;
  L_COUNT_MARKER("Wasted memory", wasted);
  allocated += padded_size;
  L_COUNT_MARKER("Allocated memory", allocated);

  while(void* ptr = freelists[index]) { // There's a free space available
    if(cas((uintptr_t*)freelists + index, uintptr_t(ptr), *(uintptr_t*)ptr) == uintptr_t(ptr)) {
      unused -= padded_size;
      L_COUNT_MARKER("Unused memory", unused);
      return ptr;
    }
  }

  // No free space available: make one
  static uint8_t* next = (uint8_t*)Memory::virtual_alloc(1ull << 29);
  return (void*)atomic_add((intptr_t*)&next, padded_size);
}
void* Memory::alloc_zero(size_t size) {
  void* ptr = alloc(size);
  memset(ptr, 0, size);
  return ptr;
}
void* Memory::realloc(void* ptr, size_t oldsize, size_t newsize) {
  if(oldsize == newsize) {
    return ptr;
  } else if(newsize == 0 && oldsize > 0) {
    free(ptr, oldsize);
    return nullptr;
  }
  // If the change in size does not incur a change in freelist index,
  // then we can simply ignore the realloc
  else if(oldsize && oldsize < max_size && newsize < max_size &&
    freelist_index(oldsize) == freelist_index(newsize)) {
    wasted += oldsize - newsize;
    L_COUNT_MARKER("Wasted memory", wasted);
    return ptr;
  } else {
    void* new_ptr = alloc(newsize);
    if(ptr) {
      memcpy(new_ptr, ptr, min(oldsize, newsize));
      free(ptr, oldsize);
    }
    return new_ptr;
  }
}
void Memory::free(void* ptr, size_t size) {
  L_ASSERT(ptr != nullptr);
  if(size >= max_size) { // Big allocations go directly to the system
    virtual_free(ptr, size);
    allocated -= size;
    L_COUNT_MARKER("Allocated memory", allocated);
    return;
  }
  uintptr_t index;
  size_t padded_size;
  freelist_index_size(size, index, padded_size);
  do {
    *((void**)ptr) = freelists[index];
  } while(cas((uintptr_t*)freelists + index, *(uintptr_t*)ptr, uintptr_t(ptr)) != *(uintptr_t*)ptr);
  unused += padded_size;
  L_COUNT_MARKER("Unused memory", unused);
  wasted -= padded_size - size;
  L_COUNT_MARKER("Wasted memory", wasted);
  allocated -= padded_size;
  L_COUNT_MARKER("Allocated memory", allocated);
}
#endif
