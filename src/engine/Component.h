#pragma once

namespace L {
  class Entity;
  class Component {
  private:
    Entity* _entity;
    inline void entity(Entity* e) { _entity = e; }
  public:
    ~Component();
    inline Entity* entity() const { return _entity; }
    virtual void start() {}
    static inline void preupdates() {}

    virtual Component* clone() = 0;

    friend class Entity;
  };
}

#define L_COMPONENT(name)\
  public:\
  Component* clone() { return Pool<name>::global.construct(*this); }\
  inline void operator delete(void* p) { Pool<name>::global.deallocate(p); }

#include "Entity.h"