#include "TaskSystem.h"

#include "../container/TSQueue.h"
#include "../macros.h"
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
const uint32_t fiber_count = 16;

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
thread_local FiberSlot fibers[fiber_count];
thread_local uint32_t thread_index, current_fiber;

void fiber_func(void* arg) {
  const uintptr_t fiber_index = uintptr_t(arg);
  FiberSlot& fiber_slot(fibers[fiber_index]);
  Task& task(fiber_slot.task);
  while(true) {
    if(task.func) {
      task.func(task.data); // Execute task
      if(task.parent) task.parent->counter--; // Notify task done
      while(fiber_slot.counter) // Wait for child tasks
        TaskSystem::yield();
      task.func = nullptr;
      task_count--; // Notify task over
    }
    TaskSystem::yield();
  }
}

void thread_func(void* arg) {
  memset(fibers, 0, sizeof(fibers));
  thread_index = uint32_t(uintptr_t(arg));
  current_fiber = 0;
  for(uint32_t i(0); i<fiber_count; i++)
    fibers[i].handle = create_fiber(fiber_func, (void*)i);
  original_thread_fibers[thread_index] = convert_to_fiber();

  while(task_count>0) {
    FiberSlot& fiber_slot(fibers[current_fiber]);
    if(fiber_slot.task.func==nullptr) // Free fiber
      (thread_tasks[thread_index].pop(fiber_slot.task) || tasks.pop(fiber_slot.task)); // Fetch task
    if(fiber_slot.task.func)  // Busy fiber
      switch_to_fiber(fiber_slot.handle);
    current_fiber = (current_fiber+1)%fiber_count;
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
void TaskSystem::push(Func f, void* d, Flags flags) {
  FiberSlot* parent;
  if(initialized && !(flags&NoParent)) {
    parent = fibers+current_fiber;
    parent->counter++;
  } else parent = nullptr;
  task_count++;
  if(flags&MainThread)
    thread_tasks[0].push(Task{f,d,flags,parent});
  else tasks.push(Task{f,d,flags,parent});
}
void TaskSystem::yield() {
  switch_to_fiber(original_thread_fibers[thread_index]);
}
void TaskSystem::join() {
  while(fibers[current_fiber].counter)
    yield();
}
