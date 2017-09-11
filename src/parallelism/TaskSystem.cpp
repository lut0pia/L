#include "TaskSystem.h"

#include "../container/TSQueue.h"
#include "../macros.h"
#include "../system/System.h"

using namespace L;

const uint32_t thread_count = 4;
const uint32_t fiber_count = 16;

typedef void* FiberHandle;

namespace L {
  FiberHandle convert_to_fiber();
  FiberHandle create_fiber(void(*)(void*), void*);
  void switch_to_fiber(FiberHandle);
  void create_thread(void(*)(void*), void*);
}

struct FiberSlot {
  FiberHandle handle;
  std::atomic<uint32_t> counter;
};

struct Task {
  TaskSystem::Func func;
  void* data;
  TaskSystem::Flags flags;
  FiberSlot* parent;
};

bool initialized(false);
TSQueue<128, Task> tasks;
TSQueue<16, Task> thread_tasks[thread_count];
FiberHandle original_thread_fibers[thread_count];
std::atomic<uint32_t> task_count(0);
thread_local FiberSlot fibers[fiber_count];
thread_local uint32_t thread_index, current_fiber;

void fiber_func(void*) {
  Task task;
  while(task_count>0) {
    if(thread_tasks[thread_index].pop(task) || tasks.pop(task)) {
      task.func(task.data); // Execute task
      if(task.parent) task.parent->counter--; // Notify task done
      TaskSystem::join(); // Wait for child tasks
      task_count--; // Notify task over
    } else TaskSystem::yield();
  }
  switch_to_fiber(original_thread_fibers[thread_index]); // Go back to original context
}

void thread_func(void* arg) {
  memset(fibers, 0, sizeof(fibers));
  thread_index = uint32_t(uintptr_t(arg));
  current_fiber = 0;
  for(uint32_t i(0); i<fiber_count; i++)
    fibers[i].handle = create_fiber(fiber_func, nullptr);
  original_thread_fibers[thread_index] = convert_to_fiber();
  switch_to_fiber(fibers[0].handle);
}

void TaskSystem::init() {
  initialized = true;
  for(uintptr_t i(1); i<thread_count; i++)
    create_thread(thread_func, (void*)i);
  thread_func(nullptr);
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
  current_fiber = (current_fiber+1)%fiber_count;
  switch_to_fiber(fibers[current_fiber].handle);
}
void TaskSystem::join() {
  while(fibers[current_fiber].counter)
    yield();
}
