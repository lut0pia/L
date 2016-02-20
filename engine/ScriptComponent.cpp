#include "ScriptComponent.h"

#include "../streams/FileStream.h"

using namespace L;
using namespace Script;

ScriptComponent::ScriptComponent(){
  _context.variable(_context.symbol("component")) = (Function)[](Context& c,int params)->Var{
    if(params==2 && c.parameter(0).is<Entity*>() && c.parameter(1).is<String>()){
      return c.parameter(0).as<Entity*>()->component(c.parameter(1).as<String>());
    } else return 0;
  };
  _context.variable(_context.symbol("move")) = (Function)[](Context& c,int params)->Var{
    if(params==4 && c.parameter(0).is<Component*>())
      c.parameter(0).as<Transform*>()->move(Vector3f(c.parameter(1).get<float>(),c.parameter(2).get<float>(),c.parameter(3).get<float>()));
    return 0;
  };
}
void ScriptComponent::load(const char* filename){
  _context.read(FileStream(filename,"rb"));
  _context.variable(_context.symbol("entity")) = entity();
  _context.execute(_context.variable(_context.symbol("start")));
}
void ScriptComponent::update(){
  _context.variable(_context.symbol("delta")) = Engine::deltaSeconds();
  _context.execute(_context.variable(_context.symbol("update")));
}
