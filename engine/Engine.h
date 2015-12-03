#ifndef DEF_L_Engine
#define DEF_L_Engine

#include "../containers/Map.h"
#include "../containers/Pool.h"
#include "../containers/Ref.h"
#include "../gl/Texture.h"
#include "../time/Timer.h"

namespace L {
  class Engine {
    private:
      template <class CompType>
      static void updateAll() {
        CompType::preupdates();
        Pool<CompType>::global.foreach([](CompType& c) {c.update();});
      }
      static Set<void (*)()> _updates;
      static Map<String,Ref<GL::Texture> > _textures;
      static Timer _timer;
      static Time _deltaTime;
      static float _deltaSeconds, _fps;
      static uint _frame;
    public:
      static inline float deltaSeconds() {return _deltaSeconds;}
      static inline float fps() {return _fps;}
      static inline uint frame() {return _frame;}
      static void update();
      static const Ref<GL::Texture>& texture(const char* filepath);

      template <class CompType> inline static void addSystem() {_updates.insert(updateAll<CompType>);}
  };
}

#endif

