#include "ScriptComponent.h"

#include "../streams/FileStream.h"
#include "Camera.h"
#include "Transform.h"
#include "Collider.h"

using namespace L;
using namespace Script;

void ScriptComponent::start(){
  _context.local(FNV1A("entity")) = entity();
}
void ScriptComponent::load(const char* filename) {
  static Var startCall(Array<Var>{FNV1A("start")});
  FileStream stream(filename,"rb");
  _context.read(stream);
  _context.execute(startCall);
}
void ScriptComponent::update() {
  static Var updateCall(Array<Var>{FNV1A("update")});
  _context.variable("delta") = Engine::deltaSeconds();
  _context.execute(updateCall);
}

void ScriptComponent::init() {
  L_ONCE;
#define L_FUNCTION(name,...) Context::global(FNV1A(name)) = (Function)([](SymbolVar* stack,size_t params)->Var {__VA_ARGS__ return 0;})
#define L_COMPONENT_ADD(fname,cname) L_FUNCTION(fname,\
    if(params==1 && stack[0]->is<Entity*>())\
      return stack[0]->as<Entity*>()->add<cname>();)
#define L_COMPONENT_GET(fname,cname) L_FUNCTION(fname,\
    if(params==1 && stack[0]->is<Entity*>())\
      return stack[0]->as<Entity*>()->component<cname>();)
  // Entity ///////////////////////////////////////////////////////////////////
  Context::global(FNV1A("entity-make")) = (Function)([](SymbolVar* stack,size_t params)->Var {
    return new Entity();
  });
  Context::global(FNV1A("entity-copy")) = (Function)([](SymbolVar* stack,size_t params)->Var {
    if(params && stack[0]->is<Entity*>())
      return new Entity(stack[0]->as<Entity*>());
    return 0;
  });
  Context::global(FNV1A("entity-destroy")) = (Function)([](SymbolVar* stack,size_t params)->Var {
    if(params && stack[0]->is<Entity*>())
      delete stack[0]->as<Entity*>();
    return 0;
  });
  // Transform ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("transform-add",Transform);
  L_COMPONENT_GET("transform-get",Transform);
  Context::global(FNV1A("transform-move")) = (Function)([](SymbolVar* stack,size_t params)->Var {
    if(params==2 && stack[0]->is<Transform*>())
      stack[0]->as<Transform*>()->move(stack[1]->get<Vector3f>());
    return 0;
  });
  Context::global(FNV1A("transform-rotate")) = (Function)([](SymbolVar* stack,size_t params)->Var {
    if(params==5 && stack[0]->is<Transform*>())
      stack[0]->as<Transform*>()->rotate(Vector3f(stack[1]->get<float>(),stack[2]->get<float>(),stack[3]->get<float>()),stack[4]->get<float>());
    return 0;
  });
  // Collider ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("collider-add",Collider);
  L_COMPONENT_GET("collider-get",Collider);
  Context::global(FNV1A("collider-box")) = (Function)([](SymbolVar* stack,size_t params)->Var {
    if(params && stack[0]->is<Collider*>()){
      Vector3f size(1.f,1.f,1.f);
      if(params == 2 && stack[1]->is<Vector3f>())
        size = stack[1]->get<Vector3f>();
      stack[0]->as<Collider*>()->box(Interval3f(-size/2.f,size/2.f));
    }
    return 0;
  });
  // RigidBody ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("rigidbody-add",RigidBody);
  L_COMPONENT_GET("rigidbody-get",RigidBody);
  // Camera ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("camera-add",Camera);
  L_COMPONENT_GET("camera-get",Camera);
  Context::global(FNV1A("camera-perspective")) = (Function)([](SymbolVar* stack,size_t params)->Var {
    if(params==5 && stack[0]->is<Camera*>())
      stack[0]->as<Camera*>()->perspective(stack[1]->get<float>(),stack[2]->get<float>(),stack[3]->get<float>(),stack[4]->get<float>());
    return 0;
  });
  // Script ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("script-add",ScriptComponent);
  L_COMPONENT_GET("script-get",ScriptComponent);
  L_FUNCTION("script-load",
             if(params==2 && stack[0]->is<ScriptComponent*>())
               stack[0]->as<ScriptComponent*>()->load(stack[1]->get<String>());
  );
}