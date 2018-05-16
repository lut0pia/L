#include "L.h"
#include "interface/glsl.h"
#include "interface/ls.h"
#include "interface/lon.h"
#include "interface/midi.h"
#include "interface/stb_image.h"
#include "interface/stb_truetype.h"
#include "interface/stb_vorbis.h"
#include "interface/obj.h"
#include "interface/wav.h"

using namespace L;

void mainjob(void*) {
  Device::init();
  Network::init();
  Audio::init();

#ifdef L_DEBUG
  ScriptServer* server(nullptr);
  if(Settings::get_int("script_server",0))
    server = Memory::new_type<ScriptServer>(short(Settings::get_int("server_port", 1993)));
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
#ifdef L_DEBUG
  flush_profiling();
#endif
}
int main(int argc, const char* argv[]) {
  TypeInit();

  Engine::register_resource<Font>();
  Engine::register_resource<Material>();
  Engine::register_resource<Mesh>();
  Engine::register_resource<Program>();
  Engine::register_resource<Texture>();
  Engine::register_resource<Script::CodeFunction>();

  Resource<Program>::add_loader("glsl", glsl_loader);
  Resource<Script::CodeFunction>::add_loader("ls", ls_loader);
  Resource<Material>::add_loader("lon", lon_loader);
  Resource<Audio::MidiSequence>::add_loader("mid", midi_loader);
  Resource<Audio::MidiSequence>::add_loader("midi", midi_loader);
  Resource<Texture>::add_loader("jpeg", stb_image_loader);
  Resource<Texture>::add_loader("jpg", stb_image_loader);
  Resource<Texture>::add_loader("png", stb_image_loader);
  Resource<Texture>::add_loader("bmp", stb_image_loader);
  Resource<Font>::add_loader("ttf", stb_truetype_loader);
  Resource<AudioStream>::add_loader("ogg", stb_vorbis_loader);
  Resource<Mesh>::add_loader("obj", obj_loader);
  Resource<AudioStream>::add_loader("wav", wav_loader);

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

  {
    Script::Context ini_context;
    ini_context.executeInside(Array<Var>{Resource<Script::CodeFunction>::get("ini.ls").ref()});
  }

  const char* window_name(Settings::get_string("window_name", "L Engine Sample"));
  int window_flags((Settings::get_int("no_cursor", 0) ? Window::nocursor : 0)
                   | (Settings::get_int("loop_cursor", 0) ? Window::loopcursor : 0));

  if(Settings::get_int("fullscreen", 1))
    Window::openFullscreen(window_name, window_flags);
  else
    Window::open(window_name, Settings::get_int("resolution_x", 1024), Settings::get_int("resolution_y", 768), window_flags);

  TaskSystem::push(mainjob, nullptr, 1, TaskSystem::MainTask);
  TaskSystem::init();
  return 0;
}

#if !defined(L_DEBUG) && defined(L_WINDOWS)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  main(0, nullptr);
}
#endif
