#include "Memory.h"

#include <cstdlib>
#include <cstring>
#include "../math/math.h"

using namespace L;

#ifndef L_USE_MALLOC
# define L_USE_MALLOC 0
#endif

#if !L_USE_MALLOC
void* _freelist[32] = {};
byte* _next;
size_t _bytesLeft(0);

inline uint32_t freelistIndex(size_t size) {
  // Cannot allocate less than a pointer's size
  return clog2(max(size, sizeof(void*)));
}
#endif

void* Memory::alloc(size_t size) {
#if L_USE_MALLOC
  return malloc(size);
#else
  const uint32_t index(freelistIndex(size));
  void*& freelist(_freelist[index]);
  void* wtr(freelist);
  if(wtr) {
    freelist = *((void**)freelist);
  } else {
    const size_t trueSize(1<<index);
    if(_bytesLeft<trueSize)
      _next = (byte*)virtualAlloc(_bytesLeft = max(trueSize, 1024*1024u));
    wtr = _next;
    _next += trueSize;
    _bytesLeft -= trueSize;
  }
  return wtr;
#endif
}
void* Memory::allocZero(size_t size) {
#if L_USE_MALLOC
  return ::calloc(size);
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
  if(freelistIndex(oldsize)!=freelistIndex(newsize)) {
    void* wtr(alloc(newsize));
    if(ptr) {
      memcpy(wtr, ptr, oldsize);
      free(ptr, oldsize);
    }
    return wtr;
  } else return ptr;
#endif
}
void Memory::free(void* ptr, size_t size) {
#if L_USE_MALLOC
  ::free(ptr);
#else
  void*& freelist(_freelist[freelistIndex(size)]);
  *((void**)ptr) = freelist;
  freelist = ptr;
#endif
}
