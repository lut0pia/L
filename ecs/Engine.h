#ifndef DEF_L_ECS_Engine
#define DEF_L_ECS_Engine

#include "../containers/Set.h"
#include "Entity.h"

namespace L {
  namespace ECS {
    class Engine {
      private:
        static Pool<Entity> _entities;
        static Set<void (*)()> _updates;
      public:
        static void addUpdate(void (*)());
        static void update();
    };
  }
}

#endif

