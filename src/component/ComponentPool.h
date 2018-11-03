#pragma once

#include "../container/IterablePool.h"
#include "../dev/profiling.h"
#include "../dynamic/Type.h"
#include "../parallelism/TaskSystem.h"

namespace L {
  template<class T>
  class ComponentPool {
  protected:
    static IterablePool<T> _pool;

  public:
    static T* allocate() { return _pool.allocate(); }
    static void deallocate(T* p) { _pool.deallocate(p); }
    static size_t size() { return _pool.objects().size(); }
    template <typename Callback>
    static void iterate(Callback f) {
      if(!_pool.objects().empty()) {
        L_SCOPE_MARKERF("Iterating over %s (%d)", Type<T>::name(), _pool.objects().size());
        // Don't use iterators because pool may change
        for(uintptr_t i(0); i<_pool.objects().size(); i++)
          f(*_pool.objects()[i]);
      }
    }
    static void async_iterate(void(*f)(T&, uint32_t)) {
      if(!_pool.objects().empty()) {
        L_SCOPE_MARKERF("Iterating async over %s (%d)", Type<T>::name(), _pool.objects().size());
        struct TaskData {
          void(*f)(T&, uint32_t);
          uint32_t t, count;
        };
        const uint32_t task_count(TaskSystem::thread_count());
        TaskData* task_data = Memory::alloc_type<TaskData>(task_count);
        for(uint32_t t(0); t<task_count; t++) {
          task_data[t] = {f,t,task_count};
          TaskSystem::push([](void* p) {
            L_SCOPE_MARKERF("Iteration task for %s", Type<T>::name());
            TaskData task_data = *(TaskData*)p;
            const uint32_t t(task_data.t);
            const uintptr_t count(max(uintptr_t(1), _pool.objects().size()/task_data.count));
            const uintptr_t start(count*t);
            const uintptr_t end(t==task_data.count-1 ? _pool.objects().size() : min(_pool.objects().size(), start+count));
            for(uintptr_t i(start); i<end; i++) {
              task_data.f(*_pool.objects()[i], t);
            }
          }, task_data+t);
        }
        TaskSystem::join();
        Memory::free_type(task_data, task_count);
      }
    }
  };

  template<class T> IterablePool<T> ComponentPool<T>::_pool;
}
