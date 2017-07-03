#pragma once

#include "ComponentPool.h"
#include "../containers/Ref.h"
#include "../containers/Table.h"
#include "../gl/Texture.h"
#include "../gl/Mesh.h"
#include "../time/Timer.h"
#include "../script/Context.h"
#include "../system/Device.h"
#include "../system/Window.h"

namespace L {
  class Camera;

  template <class T> static void updateAllComponents() { ComponentPool<T>::iterate([](T& c) { c.update(); }); }
  template <class T> static void subUpdateAllComponents() { ComponentPool<T>::iterate([](T& c) { c.subUpdate(); }); }
  template <class T> static void lateUpdateAllComponents() { ComponentPool<T>::iterate([](T& c) { c.lateUpdate(); }); }
  template <class T> static void renderAllComponents(const Camera& cam) { ComponentPool<T>::iterate([&](T& c) { c.render(cam); }); }
  template <class T> static void guiAllComponents(const Camera& cam) { ComponentPool<T>::iterate([&](T& c) { c.gui(cam); }); }
  template <class T> static void windowEventAllComponents(const Window::Event& e) { ComponentPool<T>::iterate([&](T& c) { c.event(e); }); }
  template <class T> static void deviceEventAllComponents(const Device::Event& e) { ComponentPool<T>::iterate([&](T& c) { c.event(e); }); }

  class Engine {
  private:
    static Array<void(*)()> _updates, _subUpdates, _lateUpdates;
    static Array<void(*)(const Camera&)> _renders, _guis;
    static Array<void(*)(const Window::Event&)> _windowEvents;
    static Array<void(*)(const Device::Event&)> _deviceEvents;
    static Timer _timer;
    static Time _real_delta_time, _deltaTime, _accumulator, _average_frame_work_duration;
    static Time _frame_work_durations[64];
    static const Time _subDelta;
    static float _real_delta_seconds, _deltaSeconds, _subDeltaSeconds, _timescale;
    static uint32_t _frame;
  public:
    static inline float real_delta_seconds() { return _real_delta_seconds; }
    static inline float deltaSeconds() { return _deltaSeconds; }
    static inline float subDeltaSeconds() { return _subDeltaSeconds; }
    static inline float timescale() { return _timescale; }
    static inline void timescale(float ts) { _timescale = ts; }
    static inline uint32_t frame() { return _frame; }

    static void update();
    static void clear();
    template <class T> inline static void addUpdate() { _updates.push(updateAllComponents<T>); }
    template <class T> inline static void addSubUpdate() { _subUpdates.push(subUpdateAllComponents<T>); }
    template <class T> inline static void addLateUpdate() { _lateUpdates.push(lateUpdateAllComponents<T>); }
    template <class T> inline static void addRender() { _renders.push(renderAllComponents<T>); }
    template <class T> inline static void addGui() { _guis.push(guiAllComponents<T>); }
    template <class T> inline static void addWindowEvent() { _windowEvents.push(windowEventAllComponents<T>); }
    template <class T> inline static void addDeviceEvent() { _deviceEvents.push(deviceEventAllComponents<T>); }

    // Rendering
    static GL::Buffer& sharedUniform();
    static void ditherMatrix(const float* data, size_t width, size_t height);
  };
}
