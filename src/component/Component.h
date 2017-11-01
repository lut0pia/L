#pragma once

#include "ComponentPool.h"
#include "../container/Map.h"
#include "../script/Context.h"
#include "Entity.h"

#include "../system/Device.h"
#include "../system/Window.h"


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

    virtual void update_components() {}
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

    static void update_all() {}
    static void late_update_all() {}
    static void sub_update_all() {}
    static void render_all(const class Camera&) {}
    static void gui_all(const class Camera&) {}
    static void win_event_all(const Window::Event&) {}
    static void dev_event_all(const Device::Event&) {}
    static void script_registration() {}

    friend class Entity;
  };

  template <class T> void update_all_impl() { ComponentPool<T>::iterate([](T& c) { c.update(); }); }
  template <class T> void update_all_async_impl() { ComponentPool<T>::async_iterate([](T& c, uint32_t) { c.update(); }); }
  template <class T> void sub_update_all_impl() { ComponentPool<T>::iterate([](T& c) { c.sub_update(); }); }
  template <class T> void sub_update_all_async_impl() { ComponentPool<T>::async_iterate([](T& c, uint32_t) { c.sub_update(); }); }
  template <class T> void late_update_all_impl() { ComponentPool<T>::iterate([](T& c) { c.late_update(); }); }
  template <class T> void late_update_all_async_impl() { ComponentPool<T>::async_iterate([](T& c, uint32_t) { c.late_update(); }); }
  template <class T> void render_all_impl(const Camera& cam) { ComponentPool<T>::iterate([&](T& c) { c.render(cam); }); }
  template <class T> void gui_all_impl(const Camera& cam) { ComponentPool<T>::iterate([&](T& c) { c.gui(cam); }); }
  template <class T> void win_event_all_impl(const Window::Event& e) { ComponentPool<T>::iterate([&](T& c) { c.event(e); }); }
  template <class T> void dev_event_all_impl(const Device::Event& e) { ComponentPool<T>::iterate([&](T& c) { c.event(e); }); }
}

#define L_COMPONENT(name)\
  public:\
  inline void* operator new(size_t) { return ComponentPool<name>::allocate(); }\
  inline void operator delete(void* p) { ComponentPool<name>::deallocate((name*)p); }
#define L_COMPONENT_HAS_UPDATE(name) static inline void update_all() { update_all_impl<name>(); }
#define L_COMPONENT_HAS_ASYNC_UPDATE(name) static inline void update_all() { update_all_async_impl<name>(); }
#define L_COMPONENT_HAS_SUB_UPDATE(name) static inline void sub_update_all() { sub_update_all_impl<name>(); }
#define L_COMPONENT_HAS_ASYNC_SUB_UPDATE(name) static inline void sub_update_all() { sub_update_all_async_impl<name>(); }
#define L_COMPONENT_HAS_LATE_UPDATE(name) static inline void late_update_all() { late_update_all_impl<name>(); }
#define L_COMPONENT_HAS_ASYNC_LATE_UPDATE(name) static inline void late_update_all() { late_update_all_async_impl<name>(); }
#define L_COMPONENT_HAS_RENDER(name) static inline void render_all(const Camera& cam) { render_all_impl<name>(cam); }
#define L_COMPONENT_HAS_GUI(name) static inline void gui_all(const Camera& cam) { gui_all_impl<name>(cam); }
#define L_COMPONENT_HAS_WIN_EVENT(name) static inline void win_event_all(const Window::Event& e) { win_event_all_impl<name>(e); }
#define L_COMPONENT_HAS_DEV_EVENT(name) static inline void dev_event_all(const Device::Event& e) { dev_event_all_impl<name>(e); }

#define L_COMPONENT_FUNCTION(cname,fname,n,...) Script::Context::typeValue(Type<cname*>::description(),Symbol(fname)) = (Script::Function)([](Script::Context& c) {L_ASSERT(c.localCount()>=n && c.currentSelf().is<cname*>());__VA_ARGS__})
#define L_COMPONENT_METHOD(cname,fname,n,...) L_COMPONENT_FUNCTION(cname,fname,n,c.currentSelf().as<cname*>()->__VA_ARGS__;)
#define L_COMPONENT_RETURN_METHOD(cname,fname,n,...) L_COMPONENT_FUNCTION(cname,fname,n,c.returnValue() = c.currentSelf().as<cname*>()->__VA_ARGS__;)
#define L_COMPONENT_ADD(cname,fname) L_COMPONENT_FUNCTION(Entity,fname,0,c.returnValue() = c.currentSelf().as<Entity*>()->add<cname>();)
#define L_COMPONENT_GET(cname,fname) L_COMPONENT_FUNCTION(Entity,fname,0,c.returnValue() = c.currentSelf().as<Entity*>()->component<cname>();)
#define L_COMPONENT_REQUIRE(cname,fname) L_COMPONENT_FUNCTION(Entity,fname,0,c.returnValue() = c.currentSelf().as<Entity*>()->requireComponent<cname>();)
#define L_COMPONENT_COPY(cname) L_COMPONENT_FUNCTION(cname,"copy",1,if(c.local(0).is<cname*>())*(c.currentSelf().as<cname*>()) = *(c.local(0).as<cname*>());)
#define L_COMPONENT_ENTITY(cname) L_COMPONENT_RETURN_METHOD(cname,"entity",0,entity())
#define L_COMPONENT_BIND(cname,name)\
  L_COMPONENT_ADD(cname,"add-" name);\
  L_COMPONENT_GET(cname,"get-" name);\
  L_COMPONENT_REQUIRE(cname,"require-" name);\
  L_COMPONENT_ENTITY(cname);\
  L_COMPONENT_COPY(cname);\
  Type<cname*>::cancmp<>();
