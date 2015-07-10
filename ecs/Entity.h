#ifndef DEF_L_ECS_Entity
#define DEF_L_ECS_Entity

#include "../dynamic/Type.h"
#include "../containers/Pool.h"
#include "../containers/Set.h"
#include "../containers/SortedArray.h"
#include "../containers/KeyValue.h"

namespace L {
  namespace ECS {
    class Component;
    class Entity {
      private:
        static Set<void (*)()> _updates;
        SortedArray<KeyValue<const Dynamic::TypeDescription*,Component*> > _components;
        template <class CompType>
        class ComponentPool {
          public:
            static Pool<CompType> pool;
            static void updateAll() {
              pool.foreach([](CompType& c) {c.update();});
            }
        };
      public:
        static void update() {
          for(int i(0); i<_updates.size(); i++)
            _updates[i]();
        }
        static Entity* create() {
          return ComponentPool<Entity>::pool.construct();
        }
        template <class CompType>
        Set<Component*> components() {
          Set<Component*> wtr;
          int first(_components.first(keyValue(Dynamic::Type<CompType>::description(),(Component*)NULL))),
              last(_components.last(keyValue(Dynamic::Type<CompType>::description(),(Component*)NULL)));
          for(int i(first); i<last; i++)
            wtr.insert(_components[i].value());
          return wtr;
        }
        template <class CompType>
        CompType& component() {
          int first(_components.first(keyValue(Dynamic::Type<CompType>::description(),(Component*)NULL)));
          if(first>=0)
            return *((CompType*)_components[first].value());
          else throw Exception("No such component in entity");
        }

        template <class CompType, typename... Args>
        void add(Args&&... args) {
          Component* component(ComponentPool<CompType>::pool.construct(args...));
          component->entity(this);
          _components.insert(keyValue(Dynamic::Type<CompType>::description(),component)); // Link the entity to the component
        }
        template <class CompType>
        static void addSystem() {
          _updates.insert(ComponentPool<CompType>::updateAll); // Make sure the component's update is in the engine
        }
    };
    template <class CompType>
    Pool<CompType> Entity::ComponentPool<CompType>::pool;
  }
}


#endif

