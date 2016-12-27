#include "Memory.h"

#include <Windows.h>

using namespace L;

void* Memory::virtualAlloc(size_t size) {
  return VirtualAlloc(0, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
}
void Memory::virtualFree(void* ptr, size_t size) {
  VirtualFree(ptr, 0, MEM_RELEASE);
}
