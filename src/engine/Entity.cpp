#include "Entity.h"

using namespace L;

Entity::Entity(const Entity* other) {
  for(auto p : other->_components){
    p.value() = p.value()->clone();
    p.value()->entity(this);
    _components.push(p);
    p.value()->start();
  }
}
Entity::~Entity() {
  // Components remove themselves from entity on destruction
  while(!_components.empty())
    _components[0].key()->del(_components[0].value());
}

void Entity::remove(Component* c){
  for(uint32_t i(0); i<_components.size(); i++)
    if(_components[i].value()==c){
      _components.erase(i);
      return;
    }
}