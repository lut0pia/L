#ifndef DEF_L_ECS_Component
#define DEF_L_ECS_Component

#include "Entity.h"

namespace L {
  namespace ECS {
    class Component {
      private:
        Entity* _entity;
        inline void entity(Entity* e) {_entity = e;}
      public:
        inline Entity& entity() const {return *_entity;}
        void update(){}
        friend class Entity;
    };
  }
}

#endif

