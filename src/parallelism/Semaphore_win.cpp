#include "Semaphore.h"

#include <windows.h>
#include "../dev/debug.h"

using namespace L;

#define _handle (_data.as<HANDLE>())

Semaphore::Semaphore(uint32_t initial_count) {
  _handle = CreateSemaphore(NULL, initial_count, 1 << 30, NULL);
  L_ASSERT(_handle != NULL);
}
Semaphore::~Semaphore() {
  CloseHandle(_handle);
}
bool Semaphore::try_get() {
  return !WaitForSingleObject(_handle, 0);
}
void Semaphore::get() {
  WaitForSingleObject(_handle, INFINITE);
}
void Semaphore::put() {
  ReleaseSemaphore(_handle, 1, NULL);
}
