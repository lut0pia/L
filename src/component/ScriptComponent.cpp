#include "ScriptComponent.h"

#include "../engine/Engine.h"
#include "../rendering/Material.h"
#include "../engine/Resource.inl"
#include "../engine/Settings.h"
#include "NameComponent.h"
#include "../text/String.h"

using namespace L;

static const Symbol entity_symbol("entity"), update_components_symbol("update-components"), gui_symbol("gui"),
start_symbol("start"), update_symbol("update"), late_update_symbol("late-update"), event_symbol("event");

void ScriptComponent::update_components() {
  _context.self_table()[entity_symbol] = entity();

  if(!_started && _script)
    start();

  _context.try_execute_method(update_components_symbol);
}
Map<Symbol, Var> ScriptComponent::pack() const {
  return {{"script",_script}};
}
void ScriptComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "script", _script);
}
void ScriptComponent::script_registration() {
#define L_FUNCTION(name,...) ScriptContext::global(Symbol(name)) = (ScriptNativeFunction)([](ScriptContext& c) {__VA_ARGS__})
#define L_METHOD(type,name,n,...) ScriptContext::type_value(Type<type*>::description(),Symbol(name)) = (ScriptNativeFunction)([](ScriptContext& c) {L_ASSERT(c.param_count()>=n && c.current_self().is<type*>());c.current_self().as<type*>()->__VA_ARGS__;})
#define L_RETURN_METHOD(type,name,n,...) ScriptContext::type_value(Type<type*>::description(),Symbol(name)) = (ScriptNativeFunction)([](ScriptContext& c) {L_ASSERT(c.param_count()>=n && c.current_self().is<type*>());c.return_value() = c.current_self().as<type*>()->__VA_ARGS__;})
  // Engine ///////////////////////////////////////////////////////////////////
  L_FUNCTION("engine-timescale", {
    if(c.param_count()>0)
      Engine::timescale(c.param(0).get<float>());
    c.return_value() = Engine::timescale();
  });
  L_FUNCTION("engine-clear", {
    Engine::add_deferred_action({[](void*) {
      Engine::clear();
    }});
  });
  ScriptContext::global("engine-clear-and-read") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    Engine::add_deferred_action({[](void* p) {
      String* str((String*)p);
      Engine::clear();
      ScriptContext context;
      context.execute(*Resource<ScriptFunction>(*str));
      Memory::delete_type(str);
    }, Memory::new_type<String>(c.param(0).get<String>())});
  });
  L_FUNCTION("read", {
    L_ASSERT(c.param_count()==1);
    c.execute(*Resource<ScriptFunction>(c.param(0).get<String>()));
  });
  L_FUNCTION("setting", {
    L_ASSERT(c.param_count()==2);
    Settings::set(c.param(0), c.param(1));
  });
  // Entity ///////////////////////////////////////////////////////////////////
  ScriptContext::global("entity-make") = (ScriptNativeFunction)([](ScriptContext& c) {
    c.return_value() = new Entity();
  });
  ScriptContext::global("entity-copy") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count() && c.param(0).is<Entity*>())
      c.return_value() = new Entity(c.param(0).as<Entity*>());
  });
  ScriptContext::global("entity-destroy") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count() && c.param(0).is<Entity*>())
      Entity::destroy(c.param(0).as<Entity*>());
  });
  ScriptContext::global("entity-get") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count()) {
      if(NameComponent* name_component = NameComponent::find(c.param(0).get<Symbol>()))
        c.return_value() = name_component->entity();
      else c.return_value() = nullptr;
    } else c.return_value() = nullptr;
  });
  // Material ///////////////////////////////////////////////////////////////////
  L_METHOD(Material, "parent", 1, parent(c.param(0).get<String>()));
  L_METHOD(Material, "pipeline", 1, pipeline(c.param(0).get<String>()));
  L_METHOD(Material, "mesh", 1, mesh(c.param(0).get<String>()));
  L_METHOD(Material, "font", 1, font(c.param(0).get<String>()));
  L_METHOD(Material, "text", 1, text(c.param(0).get<String>()));
  L_METHOD(Material, "color", 2, color(c.param(0).get<Symbol>(), c.param(1).get<Color>()));
  L_METHOD(Material, "scalar", 2, scalar(c.param(0), c.param(1)));
  L_METHOD(Material, "texture", 2, texture(c.param(0), c.param(1).get<String>()));
  L_METHOD(Material, "vector", 2, vector(c.param(0), c.param(1)));
  L_METHOD(Material, "vertex-count", 1, vertex_count(c.param(0)));
  // Devices ///////////////////////////////////////////////////////////////////
  L_FUNCTION("get-devices", {
    auto wtr(ref<Table<Var,Var>>());
    for(const Device* device : Device::devices())
      (*wtr)[device] = true;
    c.return_value() = wtr;
  });
  L_COMPONENT_RETURN_METHOD(const Device, "get-axis", 1, axis(Device::symbol_to_axis(c.param(0))));
  L_COMPONENT_RETURN_METHOD(const Device, "get-button", 1, button(Device::symbol_to_button(c.param(0))));
  // Script ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(ScriptComponent, "script");
  L_COMPONENT_METHOD(ScriptComponent, "load", 1, load(c.param(0).get<String>()));
  L_COMPONENT_FUNCTION(ScriptComponent, "call", 1, {
    ScriptContext& context(c.current_self().as<ScriptComponent*>()->_context);
    c.return_value() = context.execute(c.param(0), &c.param(1), c.param_count()-1);
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
  _context.execute(*_script);
  _context.try_execute_method(start_symbol);
}
void ScriptComponent::update() {
  _context.try_execute_method(update_symbol);
}
void ScriptComponent::late_update() {
  _context.try_execute_method(late_update_symbol);
}
void ScriptComponent::event(const Device::Event& e) {
  auto table(ref<Table<Var, Var>>());
  (*table)[Symbol("device")] = e.device;
  (*table)[Symbol("button")] = Device::button_to_symbol(e.button);
  (*table)[Symbol("pressed")] = bool(e.pressed);
  event(table);
}
void ScriptComponent::event(const Ref<Table<Var, Var>>&e) {
  _context.try_execute_method(event_symbol, {e});
}
void ScriptComponent::gui(const Camera& c) {
  _context.try_execute_method(gui_symbol, {(Camera*)&c});
}
