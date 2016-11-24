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

    friend class Entity;
  };
}

#define L_COMPONENT(name)\
  public:\
  inline void* operator new(size_t) { return Pool<name>::global.allocate(); }\
  inline void operator delete(void* p) { Pool<name>::global.deallocate(p); }
