#include "TaskSystem.h"

#include "../containers/TSQueue.h"
#include "../macros.h"
#include "../system/System.h"

using namespace L;

const uint32_t threadCount = 4;
const uint32_t fiberCount = 16;

#ifdef L_WINDOWS
typedef LPVOID FiberHandle;
#endif

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
TSQueue<16, Task> threadTasks[threadCount];
std::atomic<uint32_t> taskCount(0);
thread_local FiberSlot fibers[fiberCount];
thread_local uint32_t threadIndex, currentFiber;

#ifdef L_WINDOWS
VOID __stdcall fiberFunc(LPVOID p) {
#else
void fiberFunc(void*) {
#endif
  Task task;
  while(taskCount>0) {
    if(threadTasks[threadIndex].pop(task) || tasks.pop(task)) {
      task.func(task.data); // Execute task
      if(task.parent) task.parent->counter--; // Notify task done
      TaskSystem::join(); // Wait for child tasks
      taskCount--; // Notify task over
    } else TaskSystem::yield();
  }
  if(currentFiber!=0) // Need to stop on thread converted fiber to exit correctly
    TaskSystem::yield();
}

void threadFunc(void* arg) {
  memset(fibers, 0, sizeof(fibers));
  threadIndex = uint32_t(arg);
  currentFiber = 0;
  // Create non-thread fibers
  for(uint32_t i(1); i<fiberCount; i++)
#ifdef L_WINDOWS
    fibers[i].handle = CreateFiber(0, fiberFunc, nullptr);
#endif
#ifdef L_WINDOWS
  fibers[0].handle = ConvertThreadToFiber(0);
#endif
  fiberFunc(nullptr);
}

void TaskSystem::init() {
  initialized = true;
  // Create threads
  for(uint32_t i(1); i<threadCount; i++)
#ifdef L_WINDOWS
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadFunc, (LPVOID)i, 0, NULL);
#endif
  threadFunc(nullptr);
}
void TaskSystem::push(Func f, void* d, Flags flags) {
  FiberSlot* parent;
  if(initialized && !(flags&NoParent)) {
    parent = fibers+currentFiber;
    parent->counter++;
  } else parent = nullptr;
  taskCount++;
  if(flags&MainThread)
    threadTasks[0].push(Task{f,d,flags,parent});
  else tasks.push(Task{f,d,flags,parent});
}
void TaskSystem::yield() {
  currentFiber = (currentFiber+1)%fiberCount;
#ifdef L_WINDOWS
  SwitchToFiber(fibers[currentFiber].handle);
#endif
}
void TaskSystem::join() {
  while(fibers[currentFiber].counter)
    yield();
}
