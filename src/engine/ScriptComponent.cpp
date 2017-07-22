#include "ScriptComponent.h"

#include "Camera.h"
#include "Collider.h"
#include "Sprite.h"
#include "Transform.h"
#include "StaticMesh.h"
#include "NameComponent.h"
#include "Primitive.h"
#include "../text/String.h"

using namespace L;
using namespace Script;

void ScriptComponent::updateComponents() {
  _context.selfTable()[Symbol("entity")] = entity();
 
  if(!_started && !_script_path.empty())
    start();

  static const Symbol updateComponentsSymbol("update-components");
  _context.tryExecuteMethod(updateComponentsSymbol);
}
Map<Symbol, Var> ScriptComponent::pack() const {
  Map<Symbol, Var> data;
  data["script_path"] = _script_path;
  return data;
}
void ScriptComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "script_path", _script_path);
}

void ScriptComponent::load(const char* filename) {
  _script_path = filename;
  start();
}
void ScriptComponent::start() {
  _started = true;
  static const Symbol start_symbol("start");
  _context.executeInside(Array<Var>{Resource::script(_script_path)});
  _context.tryExecuteMethod(start_symbol);
}
void ScriptComponent::update() {
  static const Symbol updateSymbol("update");
  _context.tryExecuteMethod(updateSymbol);
}
void ScriptComponent::lateUpdate() {
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
  Var& typeSlot((*table)[Symbol("type")]);
  switch(e.type) { // TODO: handle other event types
    case Window::Event::MOUSEMOVE:
      typeSlot = Symbol("MOUSEMOVE");
      break;
    case Window::Event::BUTTONDOWN:
      typeSlot = Symbol("BUTTONDOWN");
      break;
    case Window::Event::BUTTONUP:
      typeSlot = Symbol("BUTTONUP");
      break;
  }
  (*table)[Symbol("button")] = Window::buttonToSymbol(e.button);
  (*table)[Symbol("x")] = e.x;
  (*table)[Symbol("y")] = e.y;
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

void ScriptComponent::init() {
  L_ONCE;
#define L_FUNCTION(name,...) Context::global(Symbol(name)) = (Function)([](Context& c) {__VA_ARGS__})
#define L_COMPONENT_FUNCTION(cname,fname,n,...) Context::typeValue(Type<cname*>::description(),Symbol(fname)) = (Function)([](Context& c) {L_ASSERT(c.localCount()>=n && c.currentSelf().is<cname*>());__VA_ARGS__})
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
  // Engine ///////////////////////////////////////////////////////////////////
  L_FUNCTION("engine-timescale", {
    if(c.localCount()>0)
      Engine::timescale(c.local(0).get<float>());
    c.returnValue() = Engine::timescale();
  });
  Context::global(Symbol("engine-gravity")) = (Function)([](Context& c) {
    if(c.localCount()>0)
      RigidBody::gravity(c.local(0).get<Vector3f>());
    c.returnValue() = RigidBody::gravity();
  });
  // Gui ///////////////////////////////////////////////////////////////////
  L_FUNCTION("draw-text", {
    Resource::font()->draw(c.local(0).get<int>(),c.local(1).get<int>(),c.local(2).get<String>());
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
  // Devices ///////////////////////////////////////////////////////////////////
  L_FUNCTION("get-devices", {
    auto wtr(ref<Table<Var,Var>>());
    for(auto&& device : Device::devices())
      (*wtr)[&device] = true;
    c.returnValue() = wtr;
  });
  L_COMPONENT_RETURN_METHOD(const Device, "get-axis", 1, axis(c.local(0).get<int>()));
  L_COMPONENT_RETURN_METHOD(const Device, "get-button", 1, button(c.local(0).get<int>()));
  // Transform ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(Transform, "transform");
  L_COMPONENT_RETURN_METHOD(Transform, "get-position", 0, absolutePosition());
  L_COMPONENT_RETURN_METHOD(Transform, "get-translation", 0, translation());
  L_COMPONENT_RETURN_METHOD(Transform, "right", 0, right());
  L_COMPONENT_RETURN_METHOD(Transform, "forward", 0, forward());
  L_COMPONENT_RETURN_METHOD(Transform, "up", 0, up());
  L_COMPONENT_METHOD(Transform, "set-translation", 1, translation(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Transform, "move", 1, move(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Transform, "rotate", 2, rotate(c.local(0).get<Vector3f>(), c.local(1).get<float>()));
  // Collider ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(Collider, "collider");
  L_COMPONENT_METHOD(Collider, "center", 1, center(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Collider, "box", 1, box(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Collider, "sphere", 1, sphere(c.local(0).get<float>()));
  Context::global(Symbol("raycast")) = (Function)([](Context& c) {
    if(c.localCount()==2 && c.local(0).is<Vector3f>() && c.local(1).is<Vector3f>()) {
      auto wtr(ref<Table<Var, Var>>());
      float t;
      (*wtr)[Symbol("collider")] = Collider::raycast(c.local(0).as<Vector3f>(), c.local(1).as<Vector3f>(), t);
      (*wtr)[Symbol("t")] = t;
      (*wtr)[Symbol("position")] = c.local(0).as<Vector3f>()+c.local(1).as<Vector3f>()*t;
      c.returnValue() = wtr;
    }
  });
  // RigidBody ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(RigidBody, "rigidbody");
  L_COMPONENT_METHOD(RigidBody, "kinematic", 1, kinematic(c.local(0).get<bool>()));
  L_COMPONENT_METHOD(RigidBody, "mass", 1, mass(c.local(0).get<float>()));
  L_COMPONENT_METHOD(RigidBody, "restitution", 1, restitution(c.local(0).get<float>()));
  L_COMPONENT_METHOD(RigidBody, "drag", 1, drag(c.local(0).get<float>()));
  L_COMPONENT_METHOD(RigidBody, "angular-drag", 1, angularDrag(c.local(0).get<float>()));
  L_COMPONENT_RETURN_METHOD(RigidBody, "get-speed", 0, velocity());
  L_COMPONENT_RETURN_METHOD(RigidBody, "get-relative-speed", 0, relativeVelocity());
  L_COMPONENT_METHOD(RigidBody, "add-speed", 1, addSpeed(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(RigidBody, "add-force", 1, addForce(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(RigidBody, "add-relative-force", 1, addRelativeForce(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(RigidBody, "add-torque", 1, addTorque(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(RigidBody, "add-relative-torque", 1, addRelativeTorque(c.local(0).get<Vector3f>()));
  // Camera ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(Camera, "camera");
  L_COMPONENT_METHOD(Camera, "perspective", 3, perspective(c.local(0).get<float>(), c.local(1).get<float>(), c.local(2).get<float>()));
  L_COMPONENT_METHOD(Camera, "ortho", 4, ortho(c.local(0).get<float>(), c.local(1).get<float>(), c.local(2).get<float>(), c.local(3).get<float>()));
  L_COMPONENT_METHOD(Camera, "viewport", 4, viewport(Interval2f(Vector2f(c.local(0).get<float>(), c.local(1).get<float>()), Vector2f(c.local(2).get<float>(), c.local(3).get<float>()))));
  // Script ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(ScriptComponent, "script");
  L_COMPONENT_METHOD(ScriptComponent, "load", 1, load(c.local(0).get<String>()));
  L_COMPONENT_FUNCTION(ScriptComponent, "call", 1, {
    L_ASSERT(c.local(0).is<Symbol>());
    Array<Var> code(Array<Var>(1, Var(Array<Var>{Symbol("self"), Script::RawSymbol{c.local(0).as<Symbol>()}})));
    for(uint32_t i(1); i<c.localCount(); i++)
      code.push(c.local(i));
    c.returnValue() = c.currentSelf().as<ScriptComponent*>()->_context.executeInside(code);
  });
  // Sprite ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(Sprite, "sprite");
  L_COMPONENT_METHOD(Sprite, "texture", 1, texture(c.local(0).get<String>()));
  L_COMPONENT_METHOD(Sprite, "vertex", 4, vertex(Interval2f(Vector2f(c.local(0).get<float>(), c.local(1).get<float>()), 
                                                            Vector2f(c.local(2).get<float>(), c.local(3).get<float>()))));
  L_COMPONENT_METHOD(Sprite, "uv", 4, uv(Interval2f(Vector2f(c.local(0).get<float>(), c.local(1).get<float>()),
                                                    Vector2f(c.local(2).get<float>(), c.local(3).get<float>()))));
  // StaticMesh ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(StaticMesh, "staticmesh");
  L_COMPONENT_METHOD(StaticMesh, "mesh", 1, mesh((const char*)c.local(0).get<String>()));
  L_COMPONENT_METHOD(StaticMesh, "texture", 1, texture((const char*)c.local(0).get<String>()));
  L_COMPONENT_METHOD(StaticMesh, "scale", 1, scale(c.local(0).get<float>()));
  // Primitive ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(Primitive, "primitive");
  L_COMPONENT_METHOD(Primitive, "center", 1, center(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Primitive, "box", 1, box(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Primitive, "sphere", 1, sphere(c.local(0).get<float>()));
  L_COMPONENT_METHOD(Primitive, "color", 1, color(c.local(0).get<Color>()));
  // Name ///////////////////////////////////////////////////////////////////
  L_COMPONENT_BIND(NameComponent, "name");
  L_COMPONENT_METHOD(NameComponent, "set", 1, name(c.local(0).get<Symbol>()));
  L_COMPONENT_METHOD(NameComponent, "get", 0, name());
}
