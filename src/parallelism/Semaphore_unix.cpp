#include "Semaphore.h"

#include <semaphore.h>

using namespace L;

#define _sem (_data.as<sem_t>())

Semaphore::Semaphore(uint32_t initial_count, uint32_t max_count) {
  sem_init(&_sem, 0, initial_count);
}
Semaphore::~Semaphore() {
  sem_destroy(&_sem);
}
bool Semaphore::try_get() {
  return sem_trywait(&_sem);
}
void Semaphore::get() {
  sem_wait(&_sem);
}
void Semaphore::put() {
  sem_post(&_sem);
}