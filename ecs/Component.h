#ifndef DEF_L_ECS_Component
#define DEF_L_ECS_Component

/*
#include "../stl.h"
#include "Entity.h"

namespace L {
  namespace ECS {
    template <class CompType>
    class Component {
      private:
        static ArrayMap<CompType> components;
        uint entityIndex;

      public:
        Component(uint ei) : entityIndex(ei) {}
        uint gEntityIndex() const {return entityIndex;}
        Entity& gEntity() const {
          return ECS::Entity::get(gEntityIndex());
        }

        static uint add(const CompType& c) {
          return components.insert(c);
        }
        static CompType& get(uint ci) {
          return components[ci];
        }
        static void updateAll() {
          for(uint i(0); i<components.size(); i++)
            if(components.has(i))
              components[i].update();
        }
    };

    template <class T> VectorMap<T> Component<T>::components;
  }
}
*/

#endif

