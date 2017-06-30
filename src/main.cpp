#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>

using namespace L;

void mainjob(void*) {
  TypeInit();
  Settings::load_file("settings.ini");

  const char* window_name(Settings::get_symbol("window_name", "L Engine"));
  int window_flags((Settings::get_int("no_cursor", 0) ? Window::nocursor : 0)
                   | (Settings::get_int("loop_cursor", 0) ? Window::loopcursor : 0));

  if(Settings::get_int("fullscreen", 1))
    Window::openFullscreen(window_name, window_flags);
  else
    Window::open(window_name, Settings::get_int("resolution_x", 1024), Settings::get_int("resolution_y", 768), window_flags);

  Device::init();
  Network::init();
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  Engine::addWindowEvent<Camera>();
  Engine::addWindowEvent<ScriptComponent>();
  Engine::addDeviceEvent<ScriptComponent>();
  Engine::addUpdate<RigidBody>();
  Engine::addUpdate<SpriteAnimator>();
  Engine::addUpdate<ScriptComponent>();
  Engine::addRender<Primitive>();
  Engine::addRender<Sprite>();
  Engine::addRender<StaticMesh>();
  if(Settings::get_int("render_collider", 0))
    Engine::addRender<Collider>();
  Engine::addSubUpdate<RigidBody>();
  Engine::addSubUpdate<Collider>();
  Engine::addLateUpdate<ScriptComponent>();
  Engine::addGui<ScriptComponent>();
  {
    ScriptComponent::init();
    Script::Context startupContext;
    startupContext.executeInside(Array<Var>{Resource::script("startup.ls")});
  }
  const uint32_t bs(32);
  float bayer[bs*bs];
  Engine::ditherMatrix(bayerMatrix(bs, bs, bayer), bs, bs);
  while(Window::loop())
    Engine::update();
  Engine::clear();
}
int main(int argc, const char* argv[]) {
  TaskSystem::push(mainjob, nullptr, TaskSystem::MainThread);
  TaskSystem::init();
  return 0;
}
