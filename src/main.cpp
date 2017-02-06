#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>

using namespace L;

int main(int argc,const char* argv[]) {
  new STB();
  new OBJ();
  TypeInit();
  Window::openFullscreen("Sample",Window::nocursor|Window::loopcursor);
  Device::init();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  Engine::addWindowEvent<ScriptComponent>();
  Engine::addDeviceEvent<ScriptComponent>();
  Engine::addUpdate<RigidBody>();
  Engine::addUpdate<SpriteAnimator>();
  Engine::addUpdate<ScriptComponent>();
  Engine::addRender<Primitive>();
  Engine::addRender<Sprite>();
  Engine::addRender<StaticMesh>();
#ifdef L_DEBUG
  Engine::addRender<Collider>();
#endif
  Engine::addSubUpdate<RigidBody>();
  Engine::addSubUpdate<Collider>();
  Engine::addLateUpdate<ScriptComponent>();
  {
    ScriptComponent::init();
    Script::Context startupContext;
    startupContext.executeDiscard(Array<Var>{Engine::script("startup.ls")});
  }
  const uint32_t bs(32);
  float bayer[bs*bs];
  Engine::ditherMatrix(bayerMatrix(bs,bs,bayer),bs,bs);
  while(Window::loop())
    Engine::update();
  Engine::clear();
  return 0;
}
