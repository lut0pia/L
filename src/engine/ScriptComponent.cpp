#include "ScriptComponent.h"

#include "../streams/FileStream.h"
#include "Transform.h"


using namespace L;
using namespace Script;

ScriptComponent::ScriptComponent() {
  L_ONCE;
  Context::global(FNV1A("entity-make")) = (Function)([](Context& c,int params)->Var {
    return new Entity();
  });
  Context::global(FNV1A("entity-destroy")) = (Function)([](Context& c,int params)->Var {
    if(params && c.parameter(0).is<Entity*>())
      delete c.parameter(0).as<Entity*>();
    return 0;
  });
  Context::global(FNV1A("entity-component")) = (Function)([](Context& c,int params)->Var {
    if(params==2 && c.parameter(0).is<Entity*>() && c.parameter(1).is<String>()) {
      return c.parameter(0).as<Entity*>()->component(c.parameter(1).as<String>());
    } else return 0;
  });
  Context::global(FNV1A("transform-move")) = (Function)([](Context& c,int params)->Var {
    if(params==4 && c.parameter(0).is<Component*>())
      c.parameter(0).as<Transform*>()->move(Vector3f(c.parameter(1).get<float>(),c.parameter(2).get<float>(),c.parameter(3).get<float>()));
    return 0;
  });
}
void ScriptComponent::start(){
  _context.pushVariable(FNV1A("entity"),entity());
}
void ScriptComponent::load(const char* filename) {
  FileStream stream(filename,"rb");
  _context.read(stream);
  _context.execute(_context.variable(FNV1A("start")));
}
void ScriptComponent::update() {
  _context.variable("delta") = Engine::deltaSeconds();
  _context.execute(_context.variable(FNV1A("update")));
}
