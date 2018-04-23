#include "Engine.h"

#include "../component/Camera.h"
#include "CullVolume.h"
#include "../dev/profiling.h"
#include "../hash.h"
#include "../rendering/GPUBuffer.h"
#include "../rendering/GL.h"
#include "../rendering/Program.h"
#include "../rendering/shader_lib.h"
#include "Resource.h"
#include "../script/Context.h"
#include "../system/Device.h"
#include "../system/Window.h"
#include "../stream/CFileStream.h"
#include "../component/AudioSourceComponent.h"

using namespace L;

Array<void(*)()> Engine::_updates, Engine::_sub_updates, Engine::_late_updates;
Array<void(*)(const Camera&)> Engine::_renders, Engine::_guis;
Array<void(*)(const L::Window::Event&)> Engine::_win_events;
Array<void(*)(const Device::Event&)> Engine::_dev_events;
Array<Engine::DeferredAction> Engine::_deferred_actions;
Timer Engine::_timer;
const Time Engine::_sub_delta(0, 10);
L::Time Engine::_real_delta_time, Engine::_delta_time, Engine::_accumulator(0), Engine::_average_frame_work_duration;
L::Time Engine::_frame_work_durations[64];
float Engine::_real_delta_seconds, Engine::_delta_seconds, Engine::_sub_delta_seconds(Engine::_sub_delta.fSeconds()), Engine::_timescale(1.f);
uint32_t Engine::_frame(0);

void Engine::update() {
  L_SCOPE_MARKER("Engine update");
  _real_delta_time = _timer.frame();
  _delta_time = min(_real_delta_time*_timescale, Time(0, 100)); // Cap delta time to avoid weird behaviour
  _real_delta_seconds = _real_delta_time.fSeconds();
  _delta_seconds = _delta_time.fSeconds();
  Script::Context::global("real-delta") = _real_delta_seconds;
  Script::Context::global("delta") = _delta_seconds;
  Script::Context::global("avg-frame-work-duration") = _average_frame_work_duration;
  Engine::shared_uniform().subData(L_SHAREDUNIFORM_FRAME, _frame);

  {
    L_SCOPE_MARKER("Window events");
    Window::Event e;
    while(Window::newEvent(e))
      for(const auto& event : _win_events)
        event(e);
  }
  {
    L_SCOPE_MARKER("Device events");
    Device::Event e;
    Device::update();
    while(Device::newEvent(e))
      for(const auto& event : _dev_events)
        event(e);
  }
  {
    L_SCOPE_MARKER("Component updates");
    for(const auto& update : _updates)
      update();
  }
  {
    L_SCOPE_MARKER("Component sub_updates");
    _accumulator += _delta_time;
    _sub_delta_seconds = _sub_delta.fSeconds();
    while(_sub_delta < _accumulator) {
      for(const auto& sub_update : _sub_updates)
        sub_update();
      _accumulator -= _sub_delta;
    }
  }
  {
    L_SCOPE_MARKER("Component late_updates");
    for(const auto& late_update : _late_updates)
      late_update();
  }

  Entity::flush_destroy_queue();

  {
    L_SCOPE_MARKER("Graphics rendering");
    ComponentPool<Camera>::iterate([](Camera& camera) {
      camera.prerender();
      CullVolume::cull(camera);
      for(auto&& render : _renders)
        render(camera);
      camera.postrender();
      for(auto&& gui : _guis)
        gui(camera);
    });
  }

  {
    L_SCOPE_MARKER("Audio rendering");
    static void* buffer;
    static uint32_t frame_count;
    Audio::acquire_buffer(buffer, frame_count);
    if(frame_count) {
      ComponentPool<AudioSourceComponent>::iterate([](AudioSourceComponent& asc) {
        asc.render(buffer, frame_count);
      });
      Audio::commit_buffer();
    }
  }

  { // Flush deferred actions
    L_SCOPE_MARKER("Deferred actions");
    for(const DeferredAction& deferred_action : _deferred_actions)
      deferred_action.func(deferred_action.data);
    _deferred_actions.clear();
  }

  // Compute work duration
  _frame_work_durations[_frame%L_COUNT_OF(_frame_work_durations)] = _timer.since();
  _average_frame_work_duration = 0;
  for(const Time& duration : _frame_work_durations)
    _average_frame_work_duration += duration;
  _average_frame_work_duration /= L_COUNT_OF(_frame_work_durations);

  {
    L_SCOPE_MARKER("Buffer swap");
    Window::swapBuffers();
  }
  _frame++;
}
void Engine::clear() {
  Entity::clear();
}

GPUBuffer& Engine::shared_uniform() {
  static GPUBuffer u(GL_UNIFORM_BUFFER, L_SHAREDUNIFORM_SIZE, nullptr, GL_DYNAMIC_DRAW, 0);
  return u;
}
void Engine::dither_matrix(const float* data, size_t width, size_t height) {
  shared_uniform().subData(L_SHAREDUNIFORM_DITHERMATRIXSIZE, 4, &width);
  shared_uniform().subData(L_SHAREDUNIFORM_DITHERMATRIXSIZE+4, 4, &height);
  shared_uniform().subData(L_SHAREDUNIFORM_DITHERMATRIX, width*height*4, data);
}
