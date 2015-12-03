#ifndef DEF_L_Component
#define DEF_L_Component

namespace L {
  class Entity;
  class Component {
    private:
      Entity* _entity;
      inline void entity(Entity* e) {_entity = e;}
    public:
      inline Entity& entity() const {return *_entity;}
      inline void start() {}
      static inline void preupdates() {}
      inline void update() {}
      friend class Entity;
  };
}

#endif

