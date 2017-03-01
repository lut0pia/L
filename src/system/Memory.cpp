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
void* _freelist[128] = {};
byte* _next;
size_t _bytesLeft(0);
size_t _allocated(0), _unused(0), _wasted(0);

// Cannot allocate less than 8 bytes for alignment purposes
inline uint32_t freelistIndex(size_t size) {
  return (size<=512) ? (((size+7)/8)-1) : (clog2(size)+55);
}
inline void freelistIndexSize(size_t size, uint32_t& index, uint32_t& trueSize) {
  if(size<=512) {
    index = ((size+7)/8)-1;
    trueSize = (index+1)*8;
  } else {
    index = clog2(size);
    trueSize = 1<<index;
    index += 55;
  }
}
#endif

void* Memory::alloc(size_t size) {
#if L_USE_MALLOC
  return malloc(size);
#else
  if(size>allocStep) { // Big allocations go directly to the system
    _allocated += size;
    return virtualAlloc(size);
  }
  uint32_t index, trueSize;
  freelistIndexSize(size, index, trueSize);
  void*& freelist(_freelist[index]);
  void* wtr(freelist);
  if(wtr) {
    freelist = *((void**)freelist);
    _unused -= trueSize;
  } else {
    if(_bytesLeft<trueSize)
      _next = (byte*)virtualAlloc(_bytesLeft = allocStep);
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
  L_ASSERT(!oldsize || freelistIndex(oldsize)!=freelistIndex(newsize));
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
  if(size>allocStep) { // Big allocations go directly to the system
    virtualFree(ptr, size);
    _allocated -= size;
    return;
  }
  uint32_t index, trueSize;
  freelistIndexSize(size, index, trueSize);
  void*& freelist(_freelist[index]);
  *((void**)ptr) = freelist;
  freelist = ptr;
  _unused += trueSize;
  _wasted -= trueSize-size;
  _allocated -= trueSize;
#endif
}
