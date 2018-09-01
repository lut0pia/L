#include "ScriptComponent.h"

#include "../engine/Engine.h"
#include "../rendering/Material.h"
#include "../engine/Resource.inl"
#include "../engine/Settings.h"
#include "NameComponent.h"
#include "../text/String.h"
#include "../rendering/Font.h"
#include "../rendering/shader_lib.h"

using namespace L;
using namespace Script;

void ScriptComponent::update_components() {
  _context.selfTable()[Symbol("entity")] = entity();

  if(!_started && _script)
    start();

  static const Symbol updateComponentsSymbol("update-components");
  _context.tryExecuteMethod(updateComponentsSymbol);
}
Map<Symbol, Var> ScriptComponent::pack() const {
  return{{"script",_script}};
}
void ScriptComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "script", _script);
}
void ScriptComponent::script_registration() {
#define L_FUNCTION(name,...) Context::global(Symbol(name)) = (Function)([](Context& c) {__VA_ARGS__})
#define L_METHOD(type,name,n,...) Context::typeValue(Type<type*>::description(),Symbol(name)) = (Function)([](Context& c) {L_ASSERT(c.localCount()>=n && c.currentSelf().is<type*>());c.currentSelf().as<type*>()->__VA_ARGS__;})
#define L_RETURN_METHOD(type,name,n,...) Context::typeValue(Type<type*>::description(),Symbol(name)) = (Function)([](Context& c) {L_ASSERT(c.localCount()>=n && c.currentSelf().is<type*>());c.returnValue() = c.currentSelf().as<type*>()->__VA_ARGS__;})
  // Engine ///////////////////////////////////////////////////////////////////
  L_FUNCTION("engine-timescale", {
    if(c.localCount()>0)
      Engine::timescale(c.local(0).get<float>());
    c.returnValue() = Engine::timescale();
  });
  L_FUNCTION("engine-clear", {
    Engine::add_deferred_action({[](void*) {
      Engine::clear();
    }});
  });
  L_FUNCTION("engine-clear-and-read", {
    L_ASSERT(c.localCount()==1);
    Engine::add_deferred_action({[](void* p) {
      String* str((String*)p);
      Engine::clear();
      Context context;
      context.executeInside(Array<Var>{Resource<Script::CodeFunction>(*str)});
      Memory::delete_type(str);
    }, Memory::new_type<String>(c.local(0).get<String>())});
  });
  L_FUNCTION("read", {
    L_ASSERT(c.localCount()==1);
    c.executeInside(Array<Var>{Resource<Script::CodeFunction>(c.local(0).get<String>())});
  });
  L_FUNCTION("setting", {
    L_ASSERT(c.localCount()==2);
    Settings::set(c.local(0).get<Symbol>(), c.local(1));
  });
  // Entity ///////////////////////////////////////////////////////////////////
  Context::global(Symbol("entity-make")) = (Function)([](Context& c) {
    c.returnValue() = new Entity();
  });
  Context::global(Symbol("entity-copy")) = (Function)([](Context& c) {
    if(c.localCount() && c.local(0).is<Entity*>())
      c.returnValue() = new Entity(c.local(0).as<Entity*>());
  });
  Context::global(Symbol("entity-destroy")) = (Function)([](Context& c) {
    if(c.localCount() && c.local(0).is<Entity*>())
      Entity::destroy(c.local(0).as<Entity*>());
  });
  Context::global(Symbol("entity-get")) = (Function)([](Context& c) {
    if(c.localCount()) {
      if(NameComponent* name_component = NameComponent::find(c.local(0).get<Symbol>()))
        c.returnValue() = name_component->entity();
      else c.returnValue() = nullptr;
    } else c.returnValue() = nullptr;
  });
  // Material ///////////////////////////////////////////////////////////////////
  L_METHOD(Material, "parent", 1, parent(c.local(0).get<String>()));
  L_METHOD(Material, "pipeline", 1, pipeline(c.local(0).get<String>()));
  L_METHOD(Material, "mesh", 1, mesh(c.local(0).get<String>()));
  L_METHOD(Material, "font", 1, font(c.local(0).get<String>()));
  L_METHOD(Material, "text", 1, text(c.local(0).get<String>()));
  L_METHOD(Material, "color", 2, color(c.local(0).get<Symbol>(), c.local(1).get<Color>()));
  L_METHOD(Material, "scalar", 2, scalar(c.local(0), c.local(1)));
  L_METHOD(Material, "texture", 2, texture(c.local(0), c.local(1).get<String>()));
  L_METHOD(Material, "vector", 2, vector(c.local(0), c.local(1)));
  L_METHOD(Material, "vertex-count", 1, vertex_count(c.local(0)));
  // Devices ///////////////////////////////////////////////////////////////////
  L_FUNCTION("get-devices", {
    auto wtr(ref<Table<Var,Var>>());
    for(auto&& device : Device::devices())
      (*wtr)[&device] = true;
    c.returnValue() = wtr;
  });
  L_COMPONENT_RETURN_METHOD(const Device, "get-axis", 1, axis(c.local(0).get<int>()));
  L_COMPONENT_RETURN_METHOD(const Device, "get-button", 1, button(c.local(0).get<int>()));
  // Script ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(ScriptComponent, "script");
  L_COMPONENT_METHOD(ScriptComponent, "load", 1, load(c.local(0).get<String>()));
  L_COMPONENT_FUNCTION(ScriptComponent, "call", 1, {
    Array<Var> code{c.local(0)};
    for(uint32_t i(1); i<c.localCount(); i++)
      code.push(c.local(i));
    c.returnValue() = c.currentSelf().as<ScriptComponent*>()->_context.executeInside(code);
  });
}

void ScriptComponent::load(const char* filename) {
  _script = filename;
  _script.flush();
  start();
}
void ScriptComponent::start() {
  L_ASSERT(_script);
  _started = true;
  static const Symbol start_symbol("start");
  _context.executeInside(Array<Var>{_script});
  _context.tryExecuteMethod(start_symbol);
}
void ScriptComponent::update() {
  static const Symbol updateSymbol("update");
  _context.tryExecuteMethod(updateSymbol);
}
void ScriptComponent::late_update() {
  static const Symbol lateUpdateSymbol("late-update");
  _context.tryExecuteMethod(lateUpdateSymbol);
}
void ScriptComponent::event(const Device::Event& e) {
  auto table(ref<Table<Var, Var>>());
  (*table)[Symbol("device")] = e._device;
  (*table)[Symbol("index")] = (int)e._index;
  (*table)[Symbol("pressed")] = (bool)e._pressed;
  event(table);
}
void ScriptComponent::event(const Window::Event& e) {
  auto table(ref<Table<Var, Var>>());
  (*table)[Symbol("type")] = Window::event_type_to_symbol(e.type);
  (*table)[Symbol("button")] = Window::buttonToSymbol(e.button);
  (*table)[Symbol("x")] = float(e.x);
  (*table)[Symbol("y")] = float(e.y);
  event(table);
}
void ScriptComponent::event(const Ref<Table<Var, Var>>&e) {
  static const Symbol eventSymbol("event");
  _context.tryExecuteMethod(eventSymbol, {e});
}
void ScriptComponent::gui(const Camera& c) {
  static const Symbol guiSymbol("gui");
  _context.tryExecuteMethod(guiSymbol, {(Camera*)&c});
}
