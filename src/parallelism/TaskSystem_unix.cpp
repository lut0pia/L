#include "TaskSystem.h"

#include "../system/Memory.h"
#include "../system/System.h"

#include <cstdlib>
#include <pthread.h>
#include <ucontext.h>

using namespace L;

static const size_t fiber_stack_size = 1 << 17;

static thread_local ucontext_t* current_context;
static uint8_t* memory_pool = nullptr;
static size_t memory_pool_size = 0;
static uint32_t memory_index = 0;

inline static void* local_alloc(uint32_t size) {
  if(!memory_pool) {
    memory_pool_size = (TaskSystem::thread_count() + TaskSystem::fiber_count())
      * (sizeof(ucontext_t) + fiber_stack_size + sizeof(void*) * 2);
    memory_pool = (uint8_t*)Memory::alloc(memory_pool_size);
  }
  uint32_t index;
  do {
    index = memory_index;
    L_ASSERT(index + size < memory_pool_size);
  } while(cas(&memory_index, index, (index + size)) != index);
  return (void*)(memory_pool + index);
}
template <class T> inline static T* local_alloc_type() {
  return (T*)local_alloc(sizeof(T));
}

static void* proxy(void* p) {
  void** pa = (void**)p;
  void(*f)(void*) = (void (*)(void*))pa[0];
  f(pa[1]);
  return nullptr;
}

namespace L {
  void* convert_to_fiber() {
    ucontext_t* context(local_alloc_type<ucontext_t>());
    getcontext(context);
    current_context = context;
    return context;
  }
  void* create_fiber(void(*f)(void*), void* p) {
    ucontext_t* context(local_alloc_type<ucontext_t>());
    if(getcontext(context) < 0) {
      error("getcontext error");
    }
    context->uc_stack.ss_size = fiber_stack_size;
    context->uc_stack.ss_sp = local_alloc(context->uc_stack.ss_size);
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
    void** proxy_p = (void**)local_alloc(2*sizeof(void*));
    proxy_p[0] = (void*)f;
    proxy_p[1] = p;
    pthread_create(&thread, nullptr, proxy, proxy_p);
  }
  uint32_t core_count() {
    String output;
    System::call("nproc", output);
    return atoi(output);
  }
}
