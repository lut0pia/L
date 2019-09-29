#include "Semaphore.h"

#include <semaphore.h>
#include "../system/Memory.h"

using namespace L;

#define _sem ((sem_t*)_data)

Semaphore::Semaphore(uint32_t initial_count) {
  _data = Memory::new_type<sem_t>();
  sem_init(_sem, 0, initial_count);
}
Semaphore::~Semaphore() {
  sem_destroy(_sem);
  Memory::delete_type<sem_t>(_sem);
}
bool Semaphore::try_get() {
  return sem_trywait(_sem);
}
void Semaphore::get() {
  sem_wait(_sem);
}
void Semaphore::put() {
  sem_post(_sem);
}