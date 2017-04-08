#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>

using namespace L;

int main(int argc,const char* argv[]) {
  TypeInit();
  Window::openFullscreen("Sample",Window::nocursor|Window::loopcursor);
  Device::init();
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
  Engine::addGui<ScriptComponent>();
  {
    ScriptComponent::init();
    Script::Context startupContext;
    startupContext.executeDiscard(Array<Var>{Resource::script("startup.ls")});
  }
  const uint32_t bs(32);
  float bayer[bs*bs];
  Engine::ditherMatrix(bayerMatrix(bs,bs,bayer),bs,bs);
  while(Window::loop())
    Engine::update();
  Engine::clear();
  return 0;
}
