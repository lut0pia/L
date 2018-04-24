#include "TaskSystem.h"

#include "../dev/profiling.h"
#include "../macros.h"
#include "Semaphore.h"
#include "../system/System.h"

using namespace L;

typedef void* FiberHandle;

namespace L {
  FiberHandle convert_to_fiber();
  FiberHandle create_fiber(void(*)(void*), void*);
  void switch_to_fiber(FiberHandle);
  void create_thread(void(*)(void*), void*);
  uint32_t core_count();
}

const uint32_t fiber_per_thread_count = 4;
const uint32_t actual_thread_count(core_count());
const uint32_t actual_fiber_count = actual_thread_count*fiber_per_thread_count;
Semaphore semaphore(0, actual_thread_count);

enum FiberState : uint32_t {
  Empty = 0,
  PreReady,
  Ready,
  Running,
};
struct FiberSlot {
  FiberHandle handle;
  TaskSystem::Func func;
  void* data;
  FiberSlot* parent;
  uint32_t state;
  uint32_t counter, thread_mask;
};

bool initialized(false);
FiberHandle* original_thread_fibers(Memory::alloc_type_zero<FiberHandle>(actual_thread_count));
FiberSlot* fibers(Memory::alloc_type_zero<FiberSlot>(actual_fiber_count));
thread_local uint32_t thread_index;
thread_local uint32_t current_fiber;

void atomic_add(uint32_t& counter, uint32_t value) {
#if _MSC_VER
  InterlockedAdd((LONG*)&counter, value);
#else
  __sync_fetch_and_add(&counter, value);
#endif
}

#if _MSC_VER
#pragma optimize("", off)
#endif
void yield_internal() {
  switch_to_fiber(original_thread_fibers[thread_index]);
}
#if _MSC_VER
#pragma optimize("", on)
#endif

void fiber_func(void* arg) {
  const uintptr_t fiber_index = uintptr_t(arg);
  FiberSlot& slot(fibers[fiber_index]);
  while(true) {
    if(slot.func) {
      slot.func(slot.data); // Execute task
      if(slot.parent) atomic_add(slot.parent->counter, -1); // Notify task done
      while(slot.counter) // Wait for child tasks
        yield_internal();
      slot.func = nullptr;
    }
    yield_internal();
  }
}

void thread_func(void* arg) {
  const uint32_t local_thread_index(thread_index = uint32_t(uintptr_t(arg)));
  current_fiber = 0;
  original_thread_fibers[local_thread_index] = convert_to_fiber();

  Time starve_start(Time::now());
  while(fibers[0].func) { // Exit when original task is over
    FiberSlot& slot(fibers[current_fiber]);
    if(slot.state==Ready && cas((uint32_t*)&slot.state, Ready, Running)==Ready) {
      if(slot.thread_mask&(1<<local_thread_index)) {
        switch_to_fiber(slot.handle);
        starve_start = Time::now();
      }
      slot.state = (slot.func==nullptr ? Empty : Ready);
    }

    // May sleep if unsollicited and not main thread
    if(local_thread_index>0 && Time::now()-starve_start >= Time(500)) {
      semaphore.get();
      starve_start = Time::now();
    }

    current_fiber = (current_fiber+1)%actual_fiber_count;
  }
}

void TaskSystem::init() {
  initialized = true;
  for(uint32_t i(0); i<actual_fiber_count; i++)
    fibers[i].handle = create_fiber(fiber_func, (void*)i);
  for(uintptr_t i(1); i<actual_thread_count; i++)
    create_thread(thread_func, (void*)i);
  thread_func(nullptr);
}
uint32_t TaskSystem::thread_count() {
  return actual_thread_count;
}
uint32_t TaskSystem::fiber_id() {
  return current_fiber;
}
void TaskSystem::push(Func f, void* d, uint32_t flags) {
  L_SCOPE_MARKER("Pushing task");
  FiberSlot* parent;
  if(initialized && !(flags&NoParent)) {
    parent = fibers+current_fiber;
    atomic_add(parent->counter, 1);
  } else parent = nullptr;
  uint32_t thread_mask(flags&MainThread ? 1 : -1);
  while(true) {
    for(uintptr_t i(0); i<actual_fiber_count; i++) {
      FiberSlot& slot(fibers[i]);
      if(slot.state==Empty && Empty==cas((uint32_t*)&slot.state, Empty, PreReady)) {
        slot.func = f;
        slot.data = d;
        slot.thread_mask = thread_mask;
        slot.parent = parent;
        slot.state = Ready;
        semaphore.put();
        return;
      }
    }
    yield_internal();
  }
}
void TaskSystem::yield() {
  L_SCOPE_MARKER("Yield");
  yield_internal();
}
void TaskSystem::join() {
  L_SCOPE_MARKER("Join");
  const uint32_t local_current_fiber(current_fiber);
  while(fibers[local_current_fiber].counter)
    yield_internal();
}

uint32_t TaskSystem::thread_mask() {
  return fibers[current_fiber].thread_mask;
}
void TaskSystem::thread_mask(uint32_t new_mask) {
  uint32_t& thread_mask(fibers[current_fiber].thread_mask);
  if(thread_mask != new_mask) {
    thread_mask = new_mask;
    if(!(new_mask&(1<<thread_index)))
      yield(); // Only yield if the new mask doesn't allow execution anymore
  }
}
