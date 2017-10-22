#pragma once

#include "../component/ComponentPool.h"
#include "../container/Table.h"
#include "../gl/Buffer.h"
#include "../time/Timer.h"
#include "../system/Device.h"
#include "../system/Window.h"

namespace L {
  class Engine {
  private:
    static Array<void(*)()> _updates, _late_updates, _sub_updates;
    static Array<void(*)(const class Camera&)> _renders, _guis;
    static Array<void(*)(const Window::Event&)> _win_events;
    static Array<void(*)(const Device::Event&)> _dev_events;
    static Timer _timer;
    static Time _real_delta_time, _delta_time, _accumulator, _average_frame_work_duration;
    static Time _frame_work_durations[64];
    static const Time _sub_delta;
    static float _real_delta_seconds, _delta_seconds, _sub_delta_seconds, _timescale;
    static uint32_t _frame;
  public:
    static inline float real_delta_seconds() { return _real_delta_seconds; }
    static inline const Time& delta_time() { return _delta_time; }
    static inline float delta_seconds() { return _delta_seconds; }
    static inline float sub_delta_seconds() { return _sub_delta_seconds; }
    static inline float timescale() { return _timescale; }
    static inline void timescale(float ts) { _timescale = ts; }
    static inline uint32_t frame() { return _frame; }

    static void update();
    static void clear();
    template <class T> inline static void register_resource() { _updates.push(Resource<T>::update); }
    template <class T> inline static void register_component() {
      if(T::update_all != Component::update_all) _updates.push(T::update_all);
      if(T::late_update_all != Component::late_update_all) _late_updates.push(T::late_update_all);
      if(T::sub_update_all != Component::sub_update_all) _sub_updates.push(T::sub_update_all);
      if(T::render_all != Component::render_all) _renders.push(T::render_all);
      if(T::gui_all != Component::gui_all) _guis.push(T::gui_all);
      if(T::win_event_all != Component::win_event_all) _win_events.push(T::win_event_all);
      if(T::dev_event_all != Component::dev_event_all) _dev_events.push(T::dev_event_all);
      T::script_registration();
    }

    // Rendering
    static GL::Buffer& shared_uniform();
    static void dither_matrix(const float* data, size_t width, size_t height);
  };
}
