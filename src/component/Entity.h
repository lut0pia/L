#pragma once

#include "../container/KeyValue.h"
#include "../container/IterablePool.h"
#include "../dynamic/Type.h"
#include "../engine/Engine.h"
#include "../system/Memory.h"

namespace L {
  class Component;
  class Entity {
  private:
    static IterablePool<Entity> _pool;
    static Array<Entity*> _destroy_queue;
    Array<KeyValue<const TypeDescription*,Component*> > _components;
    bool _destroyed : 1, _persistent : 1;

    void set_component_entity(Component*);

  public:
    inline Entity() : _destroyed(false), _persistent(true) {}
    Entity(const Entity* other);
    ~Entity();
    const Array<KeyValue<const TypeDescription*,Component*> >& components() const{ return _components; }

    inline void* operator new(size_t) { return _pool.allocate(); }
    inline void operator delete(void* p) { _pool.deallocate((Entity*)p); }

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
      CompType* component(new CompType());
      component->entity(this);
      _components.push(keyValue(Type<CompType>::description(),(Component*)component)); // Link the entity to the component
      updateComponents();
      return component;
    }
    void updateComponents();
    void remove(Component*);

    static void save(const char* path);
    static void load(const char* path);
    static void destroy(Entity* e);
    static void flush_destroy_queue();
    static void clear();

    friend Stream& operator<(Stream&, const Entity&);
    friend Stream& operator>(Stream&, Entity&);
  };
}
