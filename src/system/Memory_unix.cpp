#include "Memory.h"

using namespace L;

void* Memory::virtualAlloc(size_t size) {
  return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
}
void Memory::virtualFree(void* ptr, size_t size) {
  munmap(ptr, size);
}
