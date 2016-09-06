#pragma once

#include "../containers/KeyValue.h"
#include "../dynamic/Type.h"
#include "Engine.h"

namespace L {
  class Component;
  class Entity {
  private:
    Array<KeyValue<const TypeDescription*,Component*> > _components;

  public:
    inline Entity() {}
    Entity(const Entity* other);
    ~Entity();
    const auto& components() const{ return _components; }
    inline void* operator new(size_t size) { return Pool<Entity>::global.allocate(); }
    inline void operator delete(void* p) { Pool<Entity>::global.deallocate((Entity*)p); }

    template <class CompType>
    CompType* component() const {
      for(auto&& pair : _components)
        if(pair.key()==Type<CompType>::description())
          return (CompType*)pair.value();
      return 0;
    }
    Component* component(const char* name) const {
      for(uint32_t i(0); i<_components.size(); i++)
        if(!strcmp(_components[i].key()->name,name))
          return _components[i].value();
      return 0;
    }
    template <class CompType>
    CompType* requireComponent() {
      CompType* wtr(component<CompType>());
      if(wtr) return wtr;
      else return add<CompType>();
    }
    template <class CompType>
    CompType* add() {
      CompType* component(Pool<CompType>::global.construct());
      component->entity(this);
      _components.push(keyValue(Type<CompType>::description(),(Component*)component)); // Link the entity to the component
      updateComponents();
      return component;
    }
    void updateComponents();
    void remove(Component*);
  };
}
