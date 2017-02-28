#include "Memory.h"

#include <cstdlib>
#include <cstring>
#include "../math/math.h"

using namespace L;

void* operator new(size_t size) {
  L_ERROR("Using the standard new is forbidden. Make the struct/class L_ALLOCABLE.");
}

#ifndef L_USE_MALLOC
# define L_USE_MALLOC 0
#endif

#if !L_USE_MALLOC
const size_t allocStep = 1024*1024u;
void* _freelist[32] = {};
byte* _next;
size_t _bytesLeft(0);
size_t _allocated(0), _unused(0), _wasted(0);

inline uint32_t freelistIndex(size_t size) {
  // Cannot allocate less than 16 bytes for alignment purposes
  return clog2(max(size, 16u));
}
#endif

void* Memory::alloc(size_t size) {
#if L_USE_MALLOC
  return malloc(size);
#else
  const uint32_t index(freelistIndex(size));
  const size_t trueSize(1<<index);
  void*& freelist(_freelist[index]);
  void* wtr(freelist);
  if(wtr) {
    freelist = *((void**)freelist);
    _unused -= trueSize;
  } else {
    if(_bytesLeft<trueSize)
      _next = (byte*)virtualAlloc(_bytesLeft = max(trueSize, allocStep));
    wtr = _next;
    _next += trueSize;
    _bytesLeft -= trueSize;
  }
  _wasted += trueSize-size;
  _allocated += trueSize;
  return wtr;
#endif
}
void* Memory::allocZero(size_t size) {
#if L_USE_MALLOC
  return ::calloc(size, 1);
#else
  void* wtr(alloc(size));
  memset(wtr, 0, size);
  return wtr;
#endif
}
void* Memory::realloc(void* ptr, size_t oldsize, size_t newsize) {
#if L_USE_MALLOC
  return ::realloc(ptr, newsize);
#else
  if(oldsize && freelistIndex(oldsize)==freelistIndex(newsize)) // Already allocated an equivalent block
    return ptr;
  void* wtr(alloc(newsize));
  if(ptr) {
    memcpy(wtr, ptr, oldsize);
    free(ptr, oldsize);
  }
  return wtr;
#endif
}
void Memory::free(void* ptr, size_t size) {
#if L_USE_MALLOC
  ::free(ptr);
#else
  const uint32_t index(freelistIndex(size));
  const size_t trueSize(1<<index);
  if(trueSize<allocStep) { // Small allocations go to freelists
    void*& freelist(_freelist[index]);
    *((void**)ptr) = freelist;
    freelist = ptr;
    _unused += trueSize;
  } else { // Bigger allocations are actually freed
    virtualFree(ptr, size);
  }
  _wasted -= trueSize-size;
  _allocated -= trueSize;
#endif
}
