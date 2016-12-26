#include "Memory.h"

#include <Windows.h>

using namespace L;

void* Memory::virtualAlloc(size_t size) {
  return VirtualAlloc(0, size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
}
