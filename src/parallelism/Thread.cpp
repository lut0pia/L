#include "Thread.h"

using namespace L;

void Thread::proxy(Thread* thread) {
  thread->running = true;
  thread->result = thread->function(thread);
  thread->running = false;
}
Thread::Thread() : running(false), waitedFor(false) {}
Thread::Thread(Function f, const Var& a) : running(true), waitedFor(false) {
  start(f,a);
}
Thread::~Thread() {
  terminate();
}

void Thread::start(Function f, const Var& a) {
  function = f;
  arg = a;
#if defined L_WINDOWS
  threadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)proxy,(LPVOID)this,0,&threadId);
#elif defined L_UNIX
  pthread_create(&thread,NULL,(void* (*)(void*))proxy,this);
#endif
}
void Thread::waitForEnd(int maxMilliseconds) {
  if(running) {
    waitedFor = true;
#if defined L_WINDOWS
    WaitForSingleObject(threadHandle,maxMilliseconds);
#elif defined L_UNIX
    pthread_join(thread,NULL);
#endif
  }
}
void Thread::terminate() {
  if(running) {
#if defined L_WINDOWS
    TerminateThread(threadHandle,0);
#elif defined L_UNIX
    pthread_cancel(thread);
#endif
    running = false;
  }
}

