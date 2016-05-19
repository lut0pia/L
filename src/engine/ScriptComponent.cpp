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
#define L_FUNCTION(name,...) Context::global(FNV1A(name)) = (Function)([](Context& c,int params)->Var {__VA_ARGS__ return 0;})
#define L_COMPONENT_ADD(fname,cname) L_FUNCTION(fname,\
    if(params==1 && c.parameter(0).is<Entity*>())\
      return c.parameter(0).as<Entity*>()->add<cname>();)
#define L_COMPONENT_GET(fname,cname) L_FUNCTION(fname,\
    if(params==1 && c.parameter(0).is<Entity*>())\
      return c.parameter(0).as<Entity*>()->component<cname>();)
  // Entity ///////////////////////////////////////////////////////////////////
  Context::global(FNV1A("entity-make")) = (Function)([](Context& c,int params)->Var {
    return new Entity();
  });
  Context::global(FNV1A("entity-copy")) = (Function)([](Context& c,int params)->Var {
    if(params && c.parameter(0).is<Entity*>())
      return new Entity(c.parameter(0).as<Entity*>());
    return 0;
  });
  Context::global(FNV1A("entity-destroy")) = (Function)([](Context& c,int params)->Var {
    if(params && c.parameter(0).is<Entity*>())
      delete c.parameter(0).as<Entity*>();
    return 0;
  });
  // Transform ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("transform-add",Transform);
  L_COMPONENT_GET("transform-get",Transform);
  Context::global(FNV1A("transform-move")) = (Function)([](Context& c,int params)->Var {
    if(params==2 && c.parameter(0).is<Transform*>())
      c.parameter(0).as<Transform*>()->move(c.parameter(1).get<Vector3f>());
    return 0;
  });
  Context::global(FNV1A("transform-rotate")) = (Function)([](Context& c,int params)->Var {
    if(params==5 && c.parameter(0).is<Transform*>())
      c.parameter(0).as<Transform*>()->rotate(Vector3f(c.parameter(1).get<float>(),c.parameter(2).get<float>(),c.parameter(3).get<float>()),c.parameter(4).get<float>());
    return 0;
  });
  // Collider ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("collider-add",Collider);
  L_COMPONENT_GET("collider-get",Collider);
  Context::global(FNV1A("collider-box")) = (Function)([](Context& c,int params)->Var {
    Vector3f size(1.f,1.f,1.f);
    if(params == 2 && c.parameter(1).is<Vector3f>())
      size = c.parameter(1).get<Vector3f>();
    if(params && c.parameter(0).is<Collider*>())
      c.parameter(0).as<Collider*>()->box(Interval3f(-size/2.f,size/2.f));
    return 0;
  });
  // RigidBody ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("rigidbody-add",RigidBody);
  L_COMPONENT_GET("rigidbody-get",RigidBody);
  // Camera ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("camera-add",Camera);
  L_COMPONENT_GET("camera-get",Camera);
  Context::global(FNV1A("camera-perspective")) = (Function)([](Context& c,int params)->Var {
    if(params==5 && c.parameter(0).is<Camera*>())
      c.parameter(0).as<Camera*>()->perspective(c.parameter(1).get<float>(),c.parameter(2).get<float>(),c.parameter(3).get<float>(),c.parameter(4).get<float>());
    return 0;
  });
  // Script ///////////////////////////////////////////////////////////////////
  L_COMPONENT_ADD("script-add",ScriptComponent);
  L_COMPONENT_GET("script-get",ScriptComponent);
  L_FUNCTION("script-load",
             if(params==2 && c.parameter(0).is<ScriptComponent*>())
               c.parameter(0).as<ScriptComponent*>()->load(c.parameter(1).get<String>());
  );
}