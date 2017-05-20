#pragma once

#include "../containers/Array.h"

namespace L {
  template<class T>
  class ComponentPool {
  protected:
    static Array<T*> _components;

  public:
    static T* allocate() {
      T* component((T*)Memory::alloc(sizeof(T)));
      _components.push(component);
      return component;
    }
    static void deallocate(T* p) {
      Memory::free(p, sizeof(T));
      uintptr_t i(_components.find(p));
      if(i!=-1)
        _components.erase_fast(i);
    }
    template <typename Callback>
    static void iterate(Callback f) {
      for(auto e : _components)
        f(*e);
    }
    static void async_iterate(void(*f)(T&, uint32_t), uint32_t task_count) {
      struct TaskData {
        void(*f)(T&, uint32_t);
        uint32_t t, count;
      };
      TaskData* task_data = Memory::allocType<TaskData>(task_count);
      for(uint32_t t(0); t<task_count; t++) {
        task_data[t] = {f,t,task_count};
        TaskSystem::push([](void* p) {
          TaskData task_data = *(TaskData*)p;
          const uintptr_t t(task_data.t);
          const uintptr_t count(max(uintptr_t(1), _components.size()/task_data.count));
          const uintptr_t start(count*t);
          const uintptr_t end(t==task_data.count-1 ? _components.size() : min(_components.size(), start+count));
          for(uintptr_t i(start); i<end; i++) {
            task_data.f(*_components[i], t);
          }
        }, &task_data[t]);
      }
      TaskSystem::join();
      Memory::freeType(task_data, task_count);
    }
  };

  template<class T> Array<T*> ComponentPool<T>::_components;
}
