#ifndef DEF_L_Component
#define DEF_L_Component

namespace L {
  class Entity;
  class Camera;
  class Component {
    private:
      Entity* _entity;
      inline void entity(Entity* e) {_entity = e;}
    public:
      static const bool enableUpdate = false,
                        enableRender = false;
      inline Entity& entity() const {return *_entity;}
      inline void start() {}
      static inline void preupdates() {}
      inline void update() {}
      inline void render(const Camera&) {}
      friend class Entity;
  };
}

#include "Entity.h"

#endif

