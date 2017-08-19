#include <L/src/L.h>
#include <L/src/interface/stb_image.h>
#include <L/src/interface/stb_truetype.h>
#include <L/src/interface/obj.h>
#include <L/src/interface/wav.h>

using namespace L;

void mainjob(void*) {
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
    startupContext.executeInside(Array<Var>{Resource<Script::CodeFunction>::get("startup.ls").ref()});
  }

#ifdef L_DEBUG
  ScriptServer* server(nullptr);
  if(Settings::get_int("script_server",0))
    server = new(Memory::allocType<ScriptServer>())ScriptServer(short(Settings::get_int("server_port", 1993)));
#endif

  { // Generate and upload dither matrix
    const uint32_t matrix_size(32);
    float matrix[matrix_size*matrix_size];
    Engine::ditherMatrix(void_and_cluster(matrix_size, matrix_size, matrix), matrix_size, matrix_size);
  }

  while(Window::loop()) {
#ifdef L_DEBUG
    if(server)
      server->update();
#endif
    Engine::update();
  }
  Engine::clear();
}
int main(int argc, const char* argv[]) {
  TypeInit();
  Settings::load_file("settings.ini");

  const char* window_name(Settings::get_symbol("window_name", "L_Engine")); // TODO: string settings should not be symbols?
  int window_flags((Settings::get_int("no_cursor", 0) ? Window::nocursor : 0)
                   | (Settings::get_int("loop_cursor", 0) ? Window::loopcursor : 0));

  if(Settings::get_int("fullscreen", 1))
    Window::openFullscreen(window_name, window_flags);
  else
    Window::open(window_name, Settings::get_int("resolution_x", 1024), Settings::get_int("resolution_y", 768), window_flags);

  TaskSystem::push(mainjob, nullptr, TaskSystem::MainThread);
  TaskSystem::init();
  return 0;
}
