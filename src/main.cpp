#include <L/src/L.h>
#include <L/src/interface/stb.h>
#include <L/src/interface/obj.h>
#include <L/src/interface/wav.h>

using namespace L;

void mainjob(void*) {
  TypeInit();
  Settings::load_file("settings.ini");

  const char* window_name(Settings::get_symbol("window_name", "L_Engine")); // TODO: string settings should not be symbols?
  int window_flags((Settings::get_int("no_cursor", 0) ? Window::nocursor : 0)
                   | (Settings::get_int("loop_cursor", 0) ? Window::loopcursor : 0));

  if(Settings::get_int("fullscreen", 1))
    Window::openFullscreen(window_name, window_flags);
  else
    Window::open(window_name, Settings::get_int("resolution_x", 1024), Settings::get_int("resolution_y", 768), window_flags);

  Device::init();
  Network::init();
  Audio::init();
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  Engine::addWindowEvent<Camera>();
  Engine::addWindowEvent<ScriptComponent>();
  Engine::addDeviceEvent<ScriptComponent>();
  Engine::addUpdate<RigidBody>();
  Engine::addUpdate<SpriteAnimator>();
  Engine::addUpdate<ScriptComponent>();
  Engine::addUpdate<AudioSourceComponent>();
  Engine::addUpdate<AudioListenerComponent>();
  Engine::addLateUpdate<LightComponent>();
  Engine::addLateUpdate<HierarchyComponent>();
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

  Entity::save("../test.level");
  Entity::clear();
  Entity::load("../test.level");

#ifdef L_DEBUG
  ScriptServer server(short(Settings::get_int("server_port", 1993)));
#endif

  const uint32_t bs(32);
  float bayer[bs*bs];
  Engine::ditherMatrix(bayerMatrix(bs, bs, bayer), bs, bs);
  while(Window::loop()) {
#ifdef L_DEBUG
    server.update();
#endif
    Engine::update();
  }
  Engine::clear();
}
int main(int argc, const char* argv[]) {
  TaskSystem::push(mainjob, nullptr, TaskSystem::MainThread);
  TaskSystem::init();
  return 0;
}
