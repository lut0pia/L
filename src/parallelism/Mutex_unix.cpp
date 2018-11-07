#include "Mutex.h"

#include <pthread.h>

using namespace L;

#define _handle (_data.as<pthread_mutex_t>())

Mutex::Mutex() {
  pthread_mutex_init(&_handle, nullptr);
}
Mutex::~Mutex() {
  pthread_mutex_destroy(&_handle);
}
bool Mutex::try_lock() {
  return !pthread_mutex_trylock(&_handle);
}
void Mutex::lock() {
  pthread_mutex_lock(&_handle);
}
void Mutex::unlock() {
  pthread_mutex_unlock(&_handle);
}
