#pragma once

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

    virtual Component* clone() = 0;

    friend class Entity;
  };
}

#define L_COMPONENT(name)\
  public:\
  Component* clone() { return Pool<name>::global.construct(*this); }\
  inline void operator delete(void* p) { Pool<name>::global.deallocate(p); }
