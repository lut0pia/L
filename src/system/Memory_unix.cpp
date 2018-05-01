#include "Memory.h"

using namespace L;

void* Memory::virtual_alloc(size_t size) {
  return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
}
void Memory::virtual_free(void* ptr, size_t size) {
  munmap(ptr, size);
}
