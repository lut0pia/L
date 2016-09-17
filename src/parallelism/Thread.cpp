#include "Thread.h"

using namespace L;

void Thread::proxy(Thread* thread) {
  thread->_running = true;
  thread->_result = thread->_function(thread);
  thread->_running = false;
}

void Thread::start(Function f,const Var& a) {
  _function = f;
  _arg = a;
#if defined L_WINDOWS
  _threadHandle = CreateThread(nullptr,0,(LPTHREAD_START_ROUTINE)proxy,(LPVOID)this,0,&_threadId);
#elif defined L_UNIX
  pthread_create(&_thread,nullptr,(void* (*)(void*))proxy,this);
#endif
}
void Thread::wait(int maxMilliseconds) {
  if(_running) {
    _waitedFor = true;
#if defined L_WINDOWS
    WaitForSingleObject(_threadHandle,maxMilliseconds);
#elif defined L_UNIX
    pthread_join(_thread,nullptr);
#endif
  }
}
void Thread::terminate() {
  if(_running) {
#if defined L_WINDOWS
    TerminateThread(_threadHandle,0);
#elif defined L_UNIX
    pthread_cancel(_thread);
#endif
    _running = false;
  }
}
void Thread::affinity(uint32_t mask){
#if defined L_WINDOWS
  SetThreadAffinityMask(_threadHandle,mask);
#elif defined L_UNIX
#error Checkout pthread_setaffinity_np
#endif
}
