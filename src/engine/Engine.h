#pragma once

#include "../containers/Map.h"
#include "../containers/Pool.h"
#include "../containers/Ref.h"
#include "../gl/Texture.h"
#include "../gl/Mesh.h"
#include "../time/Timer.h"
#include "../system/Window.h"

namespace L {
  class Camera;
  class Engine {
  private:
    template <class CompType>
    static void updateAll() {
      for(auto&& c : Pool<CompType>::global)
        c.update();
    }
    template <class CompType>
    static void cleanupComp() {
      Pool<CompType>::global.clear();
    }
    template <class CompType>
    static void renderAll(const Camera& cam) {
      for(auto&& c : Pool<CompType>::global)
        c.render(cam);
    }
    template <class CompType>
    static void eventAll(const Window::Event& e) {
      for(auto&& c : Pool<CompType>::global)
        c.event(e);
    }
    static Array<void(*)()> _updates,_cleanups;
    static Array<void(*)(const Camera&)> _renders;
    static Array<void(*)(const Window::Event&)> _events;
    static Map<uint32_t,Ref<GL::Texture> > _textures;
    static Map<uint32_t,Ref<GL::Mesh> > _meshes;
    static Timer _timer;
    static Time _deltaTime;
    static float _deltaSeconds,_fps,_timescale;
    static uint32_t _frame;
  public:
    static inline float deltaSeconds() { return _deltaSeconds; }
    static inline float fps() { return _fps; }
    static inline float timescale() { return _timescale; }
    static inline void timescale(float ts) { _timescale = ts; }
    static inline uint32_t frame() { return _frame; }

    static void update();
    static void clear();
    template <class CompType> inline static void addUpdate() {
      _updates.push(updateAll<CompType>);
      _cleanups.push(cleanupComp<CompType>);
    }
    template <class CompType> inline static void addRender() {
      _renders.push(renderAll<CompType>);
    }
    template <class CompType> inline static void addEvent() {
      _events.push(eventAll<CompType>);
    }

    // Rendering
    static GL::Buffer& sharedUniform();
    static void ditherMatrix(const float* data,size_t width,size_t height);

    // Resources
    static const Ref<GL::Texture>& texture(const char* filepath);
    static const Ref<GL::Mesh>& mesh(const char* filepath);
  };
}
