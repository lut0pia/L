#include "Engine.h"

#include "Camera.h"
#include "../hash.h"
#include "../gl/Buffer.h"
#include "../gl/GL.h"
#include "../gl/Program.h"
#include "Resource.h"
#include "../script/Context.h"
#include "../system/Device.h"
#include "../system/Window.h"
#include "SharedUniform.h"
#include "../stream/CFileStream.h"

using namespace L;

Array<void(*)()> Engine::_updates, Engine::_subUpdates, Engine::_lateUpdates;
Array<void(*)(const Camera&)> Engine::_renders, Engine::_guis;
Array<void(*)(const L::Window::Event&)> Engine::_windowEvents;
Array<void(*)(const Device::Event&)> Engine::_deviceEvents;
Timer Engine::_timer;
const Time Engine::_subDelta(0, 10);
L::Time Engine::_real_delta_time, Engine::_deltaTime, Engine::_accumulator(0), Engine::_average_frame_work_duration;
L::Time Engine::_frame_work_durations[64];
float Engine::_real_delta_seconds, Engine::_deltaSeconds, Engine::_subDeltaSeconds(Engine::_subDelta.fSeconds()), Engine::_timescale(1.f);
uint32_t Engine::_frame(0);

void Engine::update() {
  _real_delta_time = _timer.frame();
  _deltaTime = min(_real_delta_time*_timescale, Time(0, 100)); // Cap delta time to avoid weird behaviour
  _real_delta_seconds = _real_delta_time.fSeconds();
  _deltaSeconds = _deltaTime.fSeconds();
  Script::Context::global("real-delta") = _real_delta_seconds;
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

  Entity::flush_destroy_queue();
  Resource::update();

  ComponentPool<Camera>::iterate([](Camera& camera) {
    camera.prerender();
    for(auto&& render : _renders)
      render(camera);
    camera.postrender();
    for(auto&& gui : _guis)
      gui(camera);
  });

  // Compute work duration
  _frame_work_durations[_frame%L_COUNT_OF(_frame_work_durations)] = _timer.since();
  _average_frame_work_duration = 0;
  for(const Time& duration : _frame_work_durations)
    _average_frame_work_duration += duration;
  _average_frame_work_duration /= L_COUNT_OF(_frame_work_durations);

  Window::swapBuffers();
  _frame++;
}
void Engine::clear() {
  //Pool<Entity>::global.clear();
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
