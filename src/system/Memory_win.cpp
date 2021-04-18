#include "Memory.h"

#include <Windows.h>

using namespace L;

void* Memory::virtual_alloc(size_t size) {
  void* ptr = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#if L_USE_DEBUG_ALLOC
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  // Put the allocation at the end of the page boundary
  const size_t remainder = size % si.dwPageSize;
  ptr = (uint8_t*)ptr + (remainder ? si.dwPageSize - remainder : 0);
  // Allocate a "no access" page right after the allocation
  VirtualAlloc((uint8_t*)ptr + size, si.dwPageSize, MEM_RESERVE, PAGE_NOACCESS);
#endif
  return ptr;
}
void Memory::virtual_free(void* ptr, size_t) {
  BOOL success = VirtualFree(ptr, 0, MEM_RELEASE);
  L_ASSERT(success);
}
