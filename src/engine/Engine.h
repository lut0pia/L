#pragma once

#include "../containers/Map.h"
#include "../containers/Pool.h"
#include "../containers/Ref.h"
#include "../gl/Texture.h"
#include "../gl/Mesh.h"
#include "../time/Timer.h"
#include "../system/Device.h"
#include "../system/Window.h"

namespace L {
  class Camera;
  class Engine {
  private:
    static Array<void(*)()> _updates,_subUpdates;
    static Array<void(*)(const Camera&)> _renders;
    static Array<void(*)(const Window::Event&)> _windowEvents;
    static Array<void(*)(const Device::Event&)> _deviceEvents;
    static Map<uint32_t,Ref<GL::Texture> > _textures;
    static Map<uint32_t,Ref<GL::Mesh> > _meshes;
    static Timer _timer;
    static Time _deltaTime;
    static float _deltaSeconds,_subDeltaSeconds,_fps,_timescale;
    static uint32_t _frame;
  public:
    static inline float deltaSeconds() { return _deltaSeconds; }
    static inline float subDeltaSeconds() { return _subDeltaSeconds; }
    static inline float fps() { return _fps; }
    static inline float timescale() { return _timescale; }
    static inline void timescale(float ts) { _timescale = ts; }
    static inline uint32_t frame() { return _frame; }

    static void update();
    static void clear();
    template <class T> inline static void addUpdate() { _updates.push(updateAllComponents<T>); }
    template <class T> inline static void addSubUpdate() { _subUpdates.push(subUpdateAllComponents<T>); }
    template <class T> inline static void addRender() { _renders.push(renderAllComponents<T>); }
    template <class T> inline static void addWindowEvent() { _windowEvents.push(windowEventAllComponents<T>); }
    template <class T> inline static void addDeviceEvent() { _deviceEvents.push(deviceEventAllComponents<T>); }

    // Rendering
    static GL::Buffer& sharedUniform();
    static void ditherMatrix(const float* data,size_t width,size_t height);

    // Resources
    static const Ref<GL::Texture>& texture(const char* filepath);
    static const Ref<GL::Mesh>& mesh(const char* filepath);
  };

  template <class T> static void updateAllComponents() { for(auto&& c : Pool<T>::global) c.update(); }
  template <class T> static void subUpdateAllComponents() { for(auto&& c : Pool<T>::global) c.subUpdate(); }
  template <class T> static void renderAllComponents(const Camera& cam) { for(auto&& c : Pool<T>::global) c.render(cam); }
  template <class T> static void windowEventAllComponents(const Window::Event& e) { for(auto&& c : Pool<T>::global) c.event(e); }
  template <class T> static void deviceEventAllComponents(const Device::Event& e) { for(auto&& c : Pool<T>::global) c.event(e); }
}
