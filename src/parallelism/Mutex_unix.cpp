#include "Mutex.h"

#include <pthread.h>
#include "../system/Memory.h"

using namespace L;

#define _handle ((pthread_mutex_t*)_data)

Mutex::Mutex() {
  _data = Memory::new_type<pthread_mutex_t>();
  pthread_mutex_init(_handle, nullptr);
}
Mutex::~Mutex() {
  pthread_mutex_destroy(_handle);
  Memory::delete_type<pthread_mutex_t>(_handle);
}
bool Mutex::try_lock() {
  return !pthread_mutex_trylock(_handle);
}
void Mutex::lock() {
  pthread_mutex_lock(_handle);
}
void Mutex::unlock() {
  pthread_mutex_unlock(_handle);
}
