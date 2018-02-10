#include "TaskSystem.h"

#include "../container/TSQueue.h"
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

const uint32_t actual_thread_count(min(TaskSystem::max_thread_count, core_count()));
Semaphore semaphore(0, actual_thread_count);

struct FiberSlot;

struct Task {
  TaskSystem::Func func;
  void* data;
  TaskSystem::Flags flags;
  FiberSlot* parent;
};

struct FiberSlot {
  FiberHandle handle;
  std::atomic<uint32_t> counter;
  Task task;
};

bool initialized(false);
TSQueue<128, Task> tasks;
TSQueue<16, Task> thread_tasks[TaskSystem::max_thread_count];
FiberHandle original_thread_fibers[TaskSystem::max_thread_count];
std::atomic<uint32_t> task_count(0);
thread_local FiberSlot fibers[TaskSystem::fiber_count];
thread_local uint32_t thread_index, current_fiber;

void yield_internal() {
  switch_to_fiber(original_thread_fibers[thread_index]);
}

void fiber_func(void* arg) {
  const uintptr_t fiber_index = uintptr_t(arg);
  FiberSlot& fiber_slot(fibers[fiber_index]);
  Task& task(fiber_slot.task);
  while(true) {
    if(task.func) {
      L_SCOPE_MARKER("Task");
      task.func(task.data); // Execute task
      if(task.parent) task.parent->counter--; // Notify task done
      while(fiber_slot.counter) // Wait for child tasks
        yield_internal();
      task.func = nullptr;
      task_count--; // Notify task over
    }
    yield_internal();
  }
}

void thread_func(void* arg) {
  memset(fibers, 0, sizeof(fibers));
  thread_index = uint32_t(uintptr_t(arg));
  current_fiber = 0;
  for(uint32_t i(0); i<TaskSystem::fiber_count; i++)
    fibers[i].handle = create_fiber(fiber_func, (void*)i);
  original_thread_fibers[thread_index] = convert_to_fiber();

  uint32_t local_task_count(0);
  Time starve_start;
  while(task_count>0) {
    FiberSlot& fiber_slot(fibers[current_fiber]);
    const bool had_task(fiber_slot.task.func!=nullptr);
    if(fiber_slot.task.func==nullptr) // Free fiber
      (thread_tasks[thread_index].pop(fiber_slot.task) || tasks.pop(fiber_slot.task)); // Fetch task
    if(fiber_slot.task.func)  // Busy fiber
      switch_to_fiber(fiber_slot.handle);

    if(had_task && !fiber_slot.task.func)
      local_task_count--;
    else if(!had_task && fiber_slot.task.func)
      local_task_count++;
    if(local_task_count==0 && task_count>0) {
      if(Time::now()-starve_start >= Time(500)) {
        L_SCOPE_MARKER("Sleep");
        semaphore.get();
        starve_start = Time::now();
      }
    } else starve_start = Time::now();

    current_fiber = (current_fiber+1)%TaskSystem::fiber_count;
  }
}

void TaskSystem::init() {
  initialized = true;
  for(uintptr_t i(1); i<actual_thread_count; i++)
    create_thread(thread_func, (void*)i);
  thread_func(nullptr);
}
uint32_t TaskSystem::thread_count() {
  return actual_thread_count;
}
uint32_t TaskSystem::fiber_id() {
  return current_fiber+fiber_count*thread_index;
}
void TaskSystem::push(Func f, void* d, Flags flags) {
  FiberSlot* parent;
  if(initialized && !(flags&NoParent)) {
    parent = fibers+current_fiber;
    parent->counter++;
  } else parent = nullptr;
  task_count++;
  if(flags&MainThread) {
    if(thread_index==0 && flags&AllowExec) f(d);
    else thread_tasks[0].push(Task{f,d,flags,parent});
  } else tasks.push(Task{f,d,flags,parent});
  semaphore.put();
}
void TaskSystem::yield() {
  L_SCOPE_MARKER("Yield");
  yield_internal();
}
void TaskSystem::join() {
  while(fibers[current_fiber].counter)
    yield();
}
