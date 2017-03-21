#include "Engine.h"

#include "Camera.h"
#include "../hash.h"
#include "../gl/Buffer.h"
#include "../gl/GL.h"
#include "../gl/Program.h"
#include "../script/Context.h"
#include "../system/Device.h"
#include "../system/Window.h"
#include "SharedUniform.h"

using namespace L;

Array<void(*)()> Engine::_updates, Engine::_subUpdates, Engine::_lateUpdates;
Array<void(*)(const Camera&)> Engine::_renders;
Array<void(*)(const L::Window::Event&)> Engine::_windowEvents;
Array<void(*)(const Device::Event&)> Engine::_deviceEvents;
Table<uint32_t, Ref<GL::Texture> > Engine::_textures;
Table<uint32_t, Ref<GL::Mesh> > Engine::_meshes;
Table<uint32_t, Ref<Script::CodeFunction>> Engine::_scripts;
Timer Engine::_timer;
const Time Engine::_subDelta(0, 10);
L::Time Engine::_deltaTime, Engine::_accumulator(0);
float Engine::_deltaSeconds, Engine::_subDeltaSeconds(Engine::_subDelta.fSeconds()), Engine::_fps, Engine::_timescale(1.f);
uint32_t Engine::_frame(0);

void Engine::update() {
  _deltaTime = _timer.frame();
  _fps = 1.f/_deltaTime.fSeconds();
  _deltaTime = min(_deltaTime*_timescale, Time(0, 100)); // Cap delta time to avoid weird behaviour
  _deltaSeconds = _deltaTime.fSeconds();
  Script::Context::global("delta") = _deltaSeconds;
  Engine::sharedUniform().subData(L_SHAREDUNIFORM_FRAME, _frame);
  Engine::sharedUniform().subData(L_SHAREDUNIFORM_SCREEN, Vector4f(float(Window::width()), float(Window::height())));

  {
    Window::Event e;
    while(Window::newEvent(e))
      for(auto&& event : _windowEvents)
        event(e);
  }
  {
    Device::Event e;
    Device::update();
    while(Device::newEvent(e))
      for(auto&& event : _deviceEvents)
        event(e);
  }
  for(auto&& update : _updates)
    update();
  {
    _accumulator += _deltaTime;
    _subDeltaSeconds = _subDelta.fSeconds();
    while(_subDelta < _accumulator) {
      for(auto&& subUpdate : _subUpdates)
        subUpdate();
      _accumulator -= _subDelta;
    }
  }
  for(auto&& lateUpdate : _lateUpdates)
    lateUpdate();

  Entity::flushDestroyQueue();

  for(auto&& camera : Pool<Camera>::global) {
    camera.prerender();
    for(auto&& render : _renders)
      render(camera);
    camera.postrender();
  }
  Window::swapBuffers();
  _frame++;
}
void Engine::clear() {
  Pool<Entity>::global.clear();
}

GL::Buffer& Engine::sharedUniform() {
  static GL::Buffer u(GL_UNIFORM_BUFFER, L_SHAREDUNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW, 0);
  return u;
}
void Engine::ditherMatrix(const float* data, size_t width, size_t height) {
  sharedUniform().subData(L_SHAREDUNIFORM_DITHERMATRIXSIZE, 4, &width);
  sharedUniform().subData(L_SHAREDUNIFORM_DITHERMATRIXSIZE+4, 4, &height);
  sharedUniform().subData(L_SHAREDUNIFORM_DITHERMATRIX, width*height*4, data);
}

const Ref<GL::Texture>& Engine::texture(const char* fp) {
  const uint32_t h(hash(fp));
  if(auto found = _textures.find(h)) return *found;
  else return _textures[h] = ref<GL::Texture>(Bitmap(fp));
}
const Ref<GL::Mesh>& Engine::mesh(const char* fp) {
  const uint32_t h(hash(fp));
  if(auto found = _meshes.find(h)) return *found;
  else return _meshes[h] = ref<GL::Mesh>(fp);
}
const Ref<Script::CodeFunction>& Engine::script(const char* fp) {
  const uint32_t h(hash(fp));
  if(auto found = _scripts.find(h)) return *found;
  else {
    FileStream stream(fp, "rb");
    return _scripts[h] = ref<Script::CodeFunction>(Script::Context::read(stream));
  }
}
