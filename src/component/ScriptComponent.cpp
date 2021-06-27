#include "ScriptComponent.h"

#include "../engine/debug_draw.h"
#include "../engine/Engine.h"
#include "../rendering/Material.h"
#include "../engine/Resource.inl"
#include "../engine/Settings.h"
#include "NameComponent.h"
#include "../text/String.h"

using namespace L;

static const Symbol entity_symbol("entity"), update_components_symbol("update_components"),
start_symbol("start"), update_symbol("update"), late_update_symbol("late_update"), event_symbol("event");

void ScriptComponent::update_components() {
  _context.self_table()[entity_symbol] = entity();

  if(!_started && _script.force_load()) {
    start();
  }

  _context.try_execute_method(update_components_symbol);
}
void ScriptComponent::script_registration() {
  // Engine ///////////////////////////////////////////////////////////////////
  ScriptGlobal("engine_timescale") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count()>0)
      Engine::timescale(c.param(0).get<float>());
    c.return_value() = Engine::timescale();
  });
  ScriptGlobal("engine_clear") = (ScriptNativeFunction)([](ScriptContext&) {
    Engine::add_deferred_action({[](void*) {
      Engine::clear();
    }});
  });
  register_script_function("engine_shutdown", [](ScriptContext&) {
    Window::instance()->close();
  });
  ScriptGlobal("engine_clear_and_read") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count() == 1);
    const Resource<ScriptFunction> script(c.param(0).get<String>());
    script.try_load();
    Engine::add_deferred_action({[](void* p) {
      Resource<ScriptFunction>* script_res = (Resource<ScriptFunction>*)p;
      Engine::clear();
      if(const ScriptFunction* script = script_res->force_load()) {
        ScriptContext().execute(ref<ScriptFunction>(*script));
      } else {
        warning("Could not load script %s for engine_clear_and_read", script_res->slot()->id);
      }
      Memory::delete_type(script_res);
    }, Memory::new_type<Resource<ScriptFunction>>(script)});
  });
  ScriptGlobal("read") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count() == 1);
    Resource<ScriptFunction> script_resource = c.param(0).get<String>();
    if(const ScriptFunction* script = script_resource.force_load()) {
      c.return_value() = ref<ScriptFunction>(*script);
    } else {
      c.return_value() = Var(); 
    }
  });
  ScriptGlobal("read_value") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count() == 1);
    Resource<Var> var_resource = c.param(0).get<String>();
    if(const Var* var = var_resource.force_load()) {
      c.return_value() = *var;
    } else {
      c.return_value() = Var();
    }
  });
  ScriptGlobal("setting") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==2);
    Settings::set(c.param(0), c.param(1));
  });
  // Entity ///////////////////////////////////////////////////////////////////
  ScriptGlobal("entity_make") = (ScriptNativeFunction)([](ScriptContext& c) {
    c.return_value() = Entity::create();
  });
  ScriptGlobal("entity_copy") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count() && c.param(0).is<Handle<Entity>>())
      c.return_value() = Entity::copy(c.param(0).as<Handle<Entity>>());
  });
  ScriptGlobal("entity_destroy") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count() && c.param(0).is<Handle<Entity>>())
      Entity::destroy(c.param(0).as<Handle<Entity>>());
  });
  ScriptGlobal("entity_get") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count()) {
      if(NameComponent* name_component = NameComponent::find(c.param(0).get<Symbol>()))
        c.return_value() = name_component->entity();
      else c.return_value() = nullptr;
    } else c.return_value() = nullptr;
  });
  // Material ///////////////////////////////////////////////////////////////////
  Material::script_registration();
  // Debug draw
#if !L_RLS
  L_SCRIPT_FUNCTION("debug_draw_line", 3, debug_draw_line(c.param(0), c.param(1), c.param(2)));
#endif
  // Devices ///////////////////////////////////////////////////////////////////
  ScriptGlobal("get_devices") = (ScriptNativeFunction)([](ScriptContext& c) {
    auto wtr(ref<Table<Var,Var>>());
    for(Device* device : Device::devices())
      (*wtr)[device->handle()] = true;
    c.return_value() = wtr;
  });
  L_SCRIPT_RETURN_METHOD(Device, "get_axis", 1, axis(Device::symbol_to_axis(c.param(0))));
  L_SCRIPT_RETURN_METHOD(Device, "get_button", 1, button(Device::symbol_to_button(c.param(0))));
  // Script ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(ScriptComponent, "script");
  L_SCRIPT_METHOD(ScriptComponent, "load", 1, load(c.param(0).get<String>()));
  L_SCRIPT_RETURN_METHOD(ScriptComponent, "object", 0, _context.self());
}

void ScriptComponent::load(const char* filename) {
  _script = filename;
  _script.force_load();
  start();
}
void ScriptComponent::start() {
  L_ASSERT(_script.is_loaded());
  _started = true;
  _context.execute(ref<ScriptFunction>(*_script.force_load()));
  _context.try_execute_method(start_symbol);
}
void ScriptComponent::update() {
  _context.try_execute_method(update_symbol);
}
void ScriptComponent::late_update() {
  _context.try_execute_method(late_update_symbol);
}
void ScriptComponent::event(const Ref<Table<Var, Var>>&e) {
  _context.try_execute_method(event_symbol, {e});
}
