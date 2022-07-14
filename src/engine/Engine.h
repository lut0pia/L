#pragma once

#include "../component/ComponentPool.h"
#include "../component/Component.h"
#include "../container/Table.h"
#include "../time/Timer.h"

namespace L {
  class Engine {
  public:
    struct DeferredAction{
      void(*func)(void*);
      void* data;
    };
  private:
    static Array<void(*)()> _parallel_updates, _updates, _late_updates, _sub_updates, _shutdowns;
    static Array<void(*)(const class Camera&, const RenderPassImpl*)> _renders;
    static Array<void(*)(void* frames, uint32_t frame_count)> _audio_renders;
    static Array<void(*)(const class Camera&)> _guis;
    static Array<void(*)(const struct Window::Event&)> _win_events;
    static Array<DeferredAction> _deferred_actions;
    static Timer _timer;
    static Time _real_delta_time, _delta_time, _accumulator, _average_frame_work_duration, _max_frame_work_duration;
    static Time _frame_work_durations[64];
    static Time _sub_delta;
    static float _real_delta_seconds, _delta_seconds, _timescale;
    static uint32_t _frame;
    static const char* _init_script;
  public:
    static inline float real_delta_seconds() { return _real_delta_seconds; }
    static inline const Time& delta_time() { return _delta_time; }
    static inline float delta_seconds() { return _delta_seconds; }
    static inline float sub_delta_seconds() { return _sub_delta.seconds_float(); }
    static inline float timescale() { return _timescale; }
    static inline void timescale(float ts) { _timescale = ts; }
    static inline uint32_t frame() { return _frame; }
    static inline void set_init_script(const char* init_script) { _init_script = init_script; }

    static void init();
    static void shutdown();
    static void update();
    static void clear();
    static void add_deferred_action(const DeferredAction& la) { _deferred_actions.push(la); }
    static void add_parallel_update(void(*update)()) { _parallel_updates.push(update); }
    static void add_update(void(*update)()) { _updates.push(update); }
    static void add_sub_update(void(*sub_update)()) { _sub_updates.push(sub_update); }
    static void add_late_update(void(*late_update)()) { _late_updates.push(late_update); }
    static void add_shutdown(void(*shutdown)()) { _shutdowns.push(shutdown); }
    static void add_render(void(*render)(const class Camera&, const RenderPassImpl*)) { _renders.push(render); }
    static void add_audio_render(void (*audio_render)(void*, uint32_t)) { _audio_renders.push(audio_render); }
    static void add_gui(void(*gui)(const class Camera&), bool debug = false) { debug ? _guis.push(gui) : _guis.insert(0, gui); }
    static void add_window_event(void(*win_event)(const struct Window::Event&)) { _win_events.push((void(*)(const struct Window::Event&))win_event); }
    template <class T> inline static void register_component() {
      if(T::flags & ComponentFlag::Update) _updates.push((void(*)())T::update_all);
      if(T::flags & ComponentFlag::UpdateAsync) _updates.push((void(*)())T::update_all_async);
      if(T::flags & ComponentFlag::SubUpdate) _sub_updates.push((void(*)())T::sub_update_all);
      if(T::flags & ComponentFlag::SubUpdateAsync) _sub_updates.push((void(*)())T::sub_update_all_async);
      if(T::flags & ComponentFlag::LateUpdate) _late_updates.push((void(*)())T::late_update_all);
      if(T::flags & ComponentFlag::LateUpdateAsync) _late_updates.push((void(*)())T::late_update_all_async);
      if(T::flags & ComponentFlag::Render) _renders.push((void(*)(const class Camera&, const RenderPassImpl*))T::render_all);
      if(T::flags & ComponentFlag::AudioRender) _audio_renders.push((void(*)(void* frames, uint32_t frame_count))T::audio_render_all);
      if(T::flags & ComponentFlag::GUI) _guis.push((void(*)(const class Camera&))T::gui_all);
      if(T::flags & ComponentFlag::WindowEvent) _win_events.push((void(*)(const struct Window::Event&))T::win_event_all);
      T::script_registration();
    }
  };
}
