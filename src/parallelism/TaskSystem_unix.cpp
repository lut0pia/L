#include "TaskSystem.h"

#include "../system/Memory.h"
#include "../system/System.h"

#include <pthread.h>
#include <ucontext.h>

using namespace L;

static thread_local ucontext_t* current_context;

namespace L {
  void* convert_to_fiber() {
    ucontext_t* context(Memory::alloc_type<ucontext_t>());
    getcontext(context);
    current_context = context;
    return context;
  }
  void* create_fiber(void(*f)(void*), void* p) {
    ucontext_t* context(Memory::alloc_type<ucontext_t>());
    if(getcontext(context)<0)
      error("getcontext error");
    context->uc_stack.ss_size = 1<<16;
    context->uc_stack.ss_sp = Memory::alloc(context->uc_stack.ss_size);
    makecontext(context, (void(*)(void))f, 1, p);
    return context;
  }
  void switch_to_fiber(void* f) {
    ucontext_t* old_context(current_context);
    current_context = (ucontext_t*)f;
    swapcontext(old_context, current_context);
  }
  void create_thread(void(*f)(void*), void* p) {
    pthread_t thread;
    pthread_create(&thread, nullptr, (void* (*)(void*))f, p);
  }
  uint32_t core_count() {
    String output;
    System::call("nproc", output);
    return atoi(output);
  }
}
