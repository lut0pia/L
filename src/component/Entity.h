#pragma once

#include "../container/Array.h"
#include "../container/Handle.h"
#include "../container/KeyValue.h"
#include "../container/Pool.h"
#include "../dynamic/Type.h"
#include "../system/Memory.h"

namespace L {
  class Component;
  class Entity : public Handled<Entity> {
  private:
    Array<KeyValue<const TypeDescription*, Handle<Component>>> _components;
    bool _destroyed : 1, _persistent : 1;

    inline Entity() : Handled<Entity>(this), _destroyed(false), _persistent(true) {}
    ~Entity();

  public:
    const Array<KeyValue<const TypeDescription*, Handle<Component>>>& components() const { return _components; }

    template <class CompType>
    void components(Array<CompType*>& comps) const {
      comps.clear();
      for(const auto& p : _components)
        if(p.key() == Type<CompType>::description())
          comps.push(*(Handle<CompType>*)&p.value());
    }

    template <class CompType>
    Handle<CompType> get_component() const {
      for(const auto& p : _components)
        if(p.key() == Type<CompType>::description())
          return *(Handle<CompType>*)&p.value();
      return Handle<CompType>();
    }
    Handle<Component> get_component(const char* name) const {
      for(const auto& p : _components)
        if(!strcmp(p.key()->name, name))
          return p.value();
      return Handle<Component>();
    }
    template <class CompType>
    Handle<CompType> require_component() {
      Handle<CompType> wtr(get_component<CompType>());
      if(wtr) return wtr;
      else return add_component<CompType>();
    }
    template <class CompType>
    Handle<CompType> add_component() {
      Handle<CompType> component = (new CompType())->handle();
      component->_entity = _handle;
      _components.push(keyValue(Type<CompType>::description(), *(Handle<Component>*)&component));
      update_components();
      return component;
    }
    void update_components();
    void remove(Handle<Component>);

    static Handle<Entity> create();
    static Handle<Entity> copy(Handle<Entity>);
    static void destroy(Handle<Entity> e);
    static void flush_destroy_queue();
    static void clear();

    friend Pool<Entity>;
  };

}
