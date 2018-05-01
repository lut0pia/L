#include "Memory.h"

#include <Windows.h>

using namespace L;

void* Memory::virtual_alloc(size_t size) {
  return VirtualAlloc(0, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
}
void Memory::virtual_free(void* ptr, size_t size) {
  VirtualFree(ptr, 0, MEM_RELEASE);
}
