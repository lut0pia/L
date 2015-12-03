#ifndef DEF_L_Entity
#define DEF_L_Entity

#include "Component.h"
#include "../containers/KeyValue.h"
#include "../dynamic/Type.h"
#include "Engine.h"

namespace L {
  class Entity {
    private:
      Array<KeyValue<const Dynamic::TypeDescription*,Component*> > _components;

    public:
      inline void* operator new(size_t size) {return Pool<Entity>::global.allocate();}
      inline void* operator new(size_t size, void* ptr) {return ptr;}
      inline void operator delete(void* p) {Pool<Entity>::global.deallocate((Entity*)p);}

      template <class CompType>
      CompType* component() {
        const Dynamic::TypeDescription* td(Dynamic::Type<CompType>::description());
        for(int i(0); i<_components.size(); i++)
          if(_components[i].key()==td)
            return (CompType*)_components[i].value();
        return 0;
      }
      template <class CompType>
      CompType* add() {
        CompType* component(Pool<CompType>::global.construct());
        component->entity(this);
        _components.push(keyValue(Dynamic::Type<CompType>::description(),(Component*)component)); // Link the entity to the component
        component->start();
        return component;
      }
  };
}


#endif

