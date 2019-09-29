#include "Mutex.h"

#include <windows.h>
#include "../system/Memory.h"

using namespace L;

#define _handle ((CRITICAL_SECTION*)_data)

Mutex::Mutex() {
  _data = Memory::new_type<CRITICAL_SECTION>();
  InitializeCriticalSection(_handle);
}
Mutex::~Mutex() {
  DeleteCriticalSection(_handle);
  Memory::delete_type<CRITICAL_SECTION>(_handle);
}
bool Mutex::try_lock() {
  return TryEnterCriticalSection(_handle) != FALSE;
}
void Mutex::lock() {
  EnterCriticalSection(_handle);
}
void Mutex::unlock() {
  LeaveCriticalSection(_handle);
}
