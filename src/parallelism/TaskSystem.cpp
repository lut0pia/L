#include "TaskSystem.h"

#include "../containers/TSQueue.h"
#include "../macros.h"
#include "../system/System.h"
#include "Thread.h"

using namespace L;

const size_t cacheLineSize = 64;
const uint32_t threadCount = 4;
const uint32_t fiberCount = 32;

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
TSQueue<1024, Task> tasks;
std::atomic<uint32_t> taskCount(0);
thread_local FiberSlot fibers[fiberCount];
thread_local uint32_t currentFiber;
thread_local bool mainThread(false);

#ifdef L_WINDOWS
VOID __stdcall fiberFunc(LPVOID p) {
#else
void fiberFunc(void*) {
#endif
  Task task;
  while(taskCount>0) {
    if(currentFiber==0)
      System::sleep(1);
    if(tasks.pop(task)) {
      if(task.flags&TaskSystem::Flags::MainThread && !mainThread)
        tasks.push(task); // Put task back in the queue
      else {
        task.func(task.data); // Execute task
        if(task.parent) task.parent->counter--; // Notify task done
        TaskSystem::join(); // Wait for child tasks
        taskCount--; // Notify task over
      }
    } else TaskSystem::yield();
  }
  if(currentFiber!=0) // Need to stop on thread converted fiber to exit correctly
    TaskSystem::yield();
}

Var threadFunc(Thread* thread) {
  memset(fibers, 0, sizeof(fibers));
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
  return nullptr;
}

void TaskSystem::init() {
  mainThread = initialized = true;
  // Create threads
  for(uint32_t i(1); i<threadCount; i++)
    new(Memory::allocType<Thread>()) Thread(threadFunc);
  threadFunc(nullptr);
}
void TaskSystem::push(Func f, void* d, Flags flags) {
  FiberSlot* parent;
  if(initialized && !(flags&NoParent)) {
    parent = fibers+currentFiber;
    parent->counter++;
  } else parent = nullptr;
  taskCount++;
  tasks.push(Task{f,d,flags,parent});
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
