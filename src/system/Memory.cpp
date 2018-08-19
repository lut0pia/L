#include "Memory.h"

#include <cstdlib>
#include <cstring>
#include "../math/math.h"
#include "../parallelism/Lock.h"

using namespace L;

#ifndef L_USE_MALLOC
# define L_USE_MALLOC 0
#endif

#if L_USE_MALLOC
void* Memory::alloc(size_t size) { return malloc(size); }
void* Memory::alloc_zero(size_t size) { return ::calloc(size, 1); }
void* Memory::realloc(void* ptr, size_t oldsize, size_t newsize) { return ::realloc(ptr, newsize); }
void Memory::free(void* ptr, size_t size) { ::free(ptr); }
#else
const size_t block_size = 1024*1024u;
static void* freelists[128] = {};
static uint8_t* next;
static size_t bytes_left(0);
static size_t allocated(0), unused(0), wasted(0);
static Lock lock;

// Cannot allocate less than 8 bytes for alignment purposes
inline uint32_t freelist_index(size_t size) {
  return (size<=512) ? (((size+7)/8)-1) : (clog2(size)+55);
}
inline void freelist_index_size(size_t size, uint32_t& index, uint32_t& padded_size) {
  if(size<=512) {
    index = ((size+7)/8)-1;
    padded_size = (index+1)*8;
  } else {
    index = clog2(size);
    padded_size = 1<<index;
    index += 55;
  }
}

void* Memory::alloc(size_t size) {
  uint32_t index, padded_size;
  freelist_index_size(size, index, padded_size);
  if(padded_size>block_size) { // Big allocations go directly to the system
    allocated += size;
    return virtual_alloc(size);
  }
  L_SCOPED_LOCK(lock);
  void*& freelist(freelists[index]);
  void* wtr(freelist);
  if(wtr) { // There's a free space available
    freelist = *((void**)freelist);
    unused -= padded_size;
  } else { // No free space available: make one
    if(bytes_left<padded_size) // No more space for allocation
      next = (uint8_t*)virtual_alloc(bytes_left = block_size); // Ask system for new memory block
    wtr = next;
    next += padded_size;
    bytes_left -= padded_size;
  }
  wasted += padded_size-size;
  allocated += padded_size;
  return wtr;
}
void* Memory::alloc_zero(size_t size) {
  void* wtr(alloc(size));
  memset(wtr, 0, size);
  return wtr;
}
void* Memory::realloc(void* ptr, size_t oldsize, size_t newsize) {
  L_ASSERT(!oldsize || freelist_index(oldsize)!=freelist_index(newsize));
  void* wtr(alloc(newsize));
  if(ptr) {
    memcpy(wtr, ptr, oldsize);
    free(ptr, oldsize);
  }
  return wtr;
}
void Memory::free(void* ptr, size_t size) {
  uint32_t index, padded_size;
  freelist_index_size(size, index, padded_size);
  if(padded_size>block_size) { // Big allocations go directly to the system
    virtual_free(ptr, size);
    allocated -= size;
    return;
  }
  L_SCOPED_LOCK(lock);
  void*& freelist(freelists[index]);
  *((void**)ptr) = freelist;
  freelist = ptr;
  unused += padded_size;
  wasted -= padded_size-size;
  allocated -= padded_size;
}
#endif
