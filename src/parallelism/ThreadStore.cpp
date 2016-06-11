#include "ThreadStore.h"

#include "../system.h"

using namespace L;

Var ThreadStore::garbageCollector(Thread* thread) {
  ThreadStore& ts(*thread->gArg().as<ThreadStore*>());
  while(!thread->isWaitedFor()) {
    size_t i(0);
    ts.mutex.lock();
    while(i<ts.threads.size()) {
      if(!ts.threads[i]->isRunning())
        ts.threads.erase(i);
      else i++;
    }
    ts.mutex.unlock();
    System::sleep(50);
  }
  return Var();
}
ThreadStore::ThreadStore() {
  attach(ref<Thread>(garbageCollector,this));
}
ThreadStore::~ThreadStore() {
  mutex.lock();
  for(auto&& thread : threads)
    thread->waitForEnd();
  mutex.unlock();
}

void ThreadStore::attach(const Ref<Thread>& thread) {
  mutex.lock();
  threads.push(thread);
  mutex.unlock();
}

