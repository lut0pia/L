#pragma once

#include <type_traits>

#include "ComponentPool.h"
#include "../container/Map.h"
#include "../script/script_binding.h"
#include "Entity.h"

#include "../system/Device.h"
#include "../system/Window.h"

namespace L {
  class Camera;
  class RenderPass;
  class Component : public GenericHandled {
  protected:
    Handle<Entity> _entity;
  public:
    inline Component() : GenericHandled(this) {}
    inline Component(const Component&) : GenericHandled(this) {}
    inline Component& operator=(const Component&) { return *this; }
    inline virtual ~Component() {}
    inline Handle<Entity> entity() const { return _entity; }

    virtual void update_components() {}
    virtual Map<Symbol, Var> pack() const { return Map<Symbol, Var>(); }
    virtual void unpack(const Map<Symbol, Var>&) {}

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

  enum class ComponentFlag : uint32_t {
    None = 0,
    Update = 1 << 0, UpdateAsync = 1 << 1, SubUpdate = 1 << 2, SubUpdateAsync = 1 << 3,
    LateUpdate = 1 << 4, LateUpdateAsync = 1 << 5, Render = 1 << 6, AudioRender = 1 << 7,
    GUI = 1 << 8, WindowEvent = 1 << 9, DeviceEvent = 1 << 10,
  };
  constexpr ComponentFlag operator|(ComponentFlag a, ComponentFlag b) { return ComponentFlag(uint32_t(a) | uint32_t(b)); }
  constexpr bool operator&(ComponentFlag a, ComponentFlag b) { return (uint32_t(a) & uint32_t(b)) != 0; }

  template <class T, ComponentFlag F = ComponentFlag::None>
  class TComponent : public Component {
  public:
    static const ComponentFlag flags = F;

    inline void* operator new(size_t) { return ComponentPool<T>::allocate(); }
    inline void operator delete(void* p) { ComponentPool<T>::deallocate((T*)p); }

    inline Handle<T> handle() const { return *(Handle<T>*)&generic_handle(); }
    inline void copy(Handle<T> other) {
      *(T*)this = *other;
    }

#define L_COMPONENT_OVERLOAD(FLAG, NAME, ...) \
    template <typename V = void> static typename std::enable_if<!(F & FLAG), V>::type NAME(__VA_ARGS__) {} \
    template <typename V = void> static typename std::enable_if<F & FLAG, V>::type NAME

    L_COMPONENT_OVERLOAD(ComponentFlag::Update, update_all)() { ComponentPool<T>::iterate([](T& c) { c.update(); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::UpdateAsync, update_all_async)() { ComponentPool<T>::async_iterate([](T& c, uint32_t) { c.update(); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::SubUpdate, sub_update_all)() { ComponentPool<T>::iterate([](T& c) { c.sub_update(); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::SubUpdateAsync, sub_update_all_async)() { ComponentPool<T>::async_iterate([](T& c, uint32_t) { c.sub_update(); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::LateUpdate, late_update_all)() { ComponentPool<T>::iterate([](T& c) { c.late_update(); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::LateUpdateAsync, late_update_all_async)() { ComponentPool<T>::async_iterate([](T& c, uint32_t) { c.late_update(); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::Render, render_all, const Camera&, const RenderPass&)(const Camera& cam, const RenderPass& rp) { ComponentPool<T>::iterate([&](T& c) { c.render(cam, rp); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::AudioRender, audio_render_all, void*, uint32_t)(void* frames, uint32_t fc) { ComponentPool<T>::iterate([&](T& c) { c.audio_render(frames, fc); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::GUI, gui_all, const Camera&)(const Camera& cam) { ComponentPool<T>::iterate([&](T& c) { c.gui(cam); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::WindowEvent, win_event_all, const Window::Event&)(const Window::Event& e) { ComponentPool<T>::iterate([&](T& c) { c.event(e); }); }
    L_COMPONENT_OVERLOAD(ComponentFlag::DeviceEvent, dev_event_all, const Device::Event&)(const Device::Event& e) { ComponentPool<T>::iterate([&](T& c) { c.event(e); }); }
  };
}

#define L_COMPONENT_ADD(cname, fname) L_SCRIPT_RETURN_METHOD(Entity, fname, 0, add_component<cname>())
#define L_COMPONENT_GET(cname, fname) L_SCRIPT_RETURN_METHOD(Entity, fname, 0, get_component<cname>())
#define L_COMPONENT_REQUIRE(cname, fname) L_SCRIPT_RETURN_METHOD(Entity, fname, 0, require_component<cname>())
#define L_COMPONENT_ENTITY(cname) L_SCRIPT_RETURN_METHOD(cname, "entity", 0, entity())
#define L_COMPONENT_COPY(cname) L_SCRIPT_METHOD(cname, "copy" , 1, copy(c.param(0).get<Handle<cname>>());)
#define L_COMPONENT_BIND(cname,name) \
  L_COMPONENT_ADD(cname,"add_" name); \
  L_COMPONENT_GET(cname,"get_" name); \
  L_COMPONENT_REQUIRE(cname,"require_" name); \
  L_COMPONENT_ENTITY(cname); \
  L_COMPONENT_COPY(cname); \
  Type<Handle<cname>>::cancmp<>();
