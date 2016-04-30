#include "ScriptComponent.h"

#include "../streams/FileStream.h"

using namespace L;
using namespace Script;

ScriptComponent::ScriptComponent() {
  _context.variable("component") = (Function)([](Context& c,int params)->Var {
    if(params==2 && c.parameter(0).is<Entity*>() && c.parameter(1).is<String>()) {
      return c.parameter(0).as<Entity*>()->component(c.parameter(1).as<String>());
    } else return 0;
  });
  _context.variable("move") = (Function)([](Context& c,int params)->Var {
    if(params==4 && c.parameter(0).is<Component*>())
      c.parameter(0).as<Transform*>()->move(Vector3f(c.parameter(1).get<float>(),c.parameter(2).get<float>(),c.parameter(3).get<float>()));
    return 0;
  });
}
void ScriptComponent::load(const char* filename) {
  _context.variable("entity") = entity();
  FileStream stream(filename,"rb");
  _context.read(stream);
  _context.execute(_context.variable("start"));
}
void ScriptComponent::update() {
  _context.variable("delta") = Engine::deltaSeconds();
  _context.execute(_context.variable("update"));
}
