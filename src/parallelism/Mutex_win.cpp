#include "Mutex.h"

#include <windows.h>

using namespace L;

#define _handle (_data.as<CRITICAL_SECTION>())

Mutex::Mutex() {
  InitializeCriticalSection(&_handle);
}
Mutex::~Mutex() {
  DeleteCriticalSection(&_handle);
}
bool Mutex::try_lock() {
  return TryEnterCriticalSection(&_handle)!=FALSE;
}
void Mutex::lock() {
  EnterCriticalSection(&_handle);
}
void Mutex::unlock() {
  LeaveCriticalSection(&_handle);
}
