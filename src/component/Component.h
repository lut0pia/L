#pragma once

#include "ComponentPool.h"
#include "../container/Map.h"
#include "Entity.h"

namespace L {
  class Entity;
  class Component {
  private:
    Entity* _entity;
    inline void entity(Entity* e) { _entity = e; }
  public:
    inline Component() {}
    inline Component(const Component&) {}
    inline Component& operator=(const Component&) { return *this; }
    inline ~Component() { entity()->remove(this); }
    inline Entity* entity() const { return _entity; }

    virtual void updateComponents() {}
    virtual Map<Symbol, Var> pack() const { return Map<Symbol, Var>(); }
    virtual void unpack(const Map<Symbol, Var>&) { }

    template <class T>
    void unpack_item(const Map<Symbol, Var>& data, const Symbol& symbol, T& value) {
      if(const Var* found = data.find(symbol))
        if(found->is<T>())
          value = found->as<T>();
    }

    friend inline Stream& operator<(Stream& s, const Component& c) {
      return s < c.pack();
    }
    friend inline Stream& operator>(Stream& s, Component& c) {
      Map<Symbol, Var> data;
      s > data;
      c.unpack(data);
      return s;
    }

    friend class Entity;
  };
}

#define L_COMPONENT(name)\
  public:\
  inline void* operator new(size_t) { return ComponentPool<name>::allocate(); }\
  inline void operator delete(void* p) { ComponentPool<name>::deallocate((name*)p); }
