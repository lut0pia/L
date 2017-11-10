#include <L/src/L.h>
#include <L/src/interface/glsl.h>
#include <L/src/interface/ls.h>
#include <L/src/interface/lon.h>
#include <L/src/interface/midi.h>
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
  Engine::register_resource<Font>();
  Engine::register_resource<Material>();
  Engine::register_resource<GL::Mesh>();
  Engine::register_resource<GL::Program>();
  Engine::register_resource<GL::Texture>();
  Engine::register_resource<Script::CodeFunction>();

  Engine::register_component<Transform>();
  Engine::register_component<Camera>();
  Engine::register_component<RigidBody>();
  Engine::register_component<Collider>();
  Engine::register_component<ScriptComponent>();
  Engine::register_component<AudioSourceComponent>();
  Engine::register_component<AudioListenerComponent>();
  Engine::register_component<MidiSourceComponent>();
  Engine::register_component<NameComponent>();
  Engine::register_component<LightComponent>();
  Engine::register_component<HierarchyComponent>();
  Engine::register_component<Primitive>();
  Engine::register_component<Sprite>();
  Engine::register_component<StaticMesh>();

  {
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
    Engine::dither_matrix(void_and_cluster(matrix_size, matrix_size, matrix), matrix_size, matrix_size);
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

#if !defined(L_DEBUG) && defined(L_WINDOWS)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  main(0, nullptr);
}
#endif
