#include "Entity.h"

#include "Component.h"

using namespace L;

Array<Entity*> Entity::_destroyQueue;

Entity::Entity(const Entity* other) {
  for(auto p : other->_components){
    p.value() = p.value()->clone();
    p.value()->entity(this);
    _components.push(p);
    p.value()->updateComponents();
  }
}
Entity::~Entity() {
  // Components remove themselves from entity on destruction
  while(!_components.empty())
    _components[0].key()->del(_components[0].value());
}

void Entity::updateComponents(){
  for(auto&& c : _components)
    c.value()->updateComponents();
}
void Entity::remove(Component* c){
  for(uint32_t i(0); i<_components.size(); i++)
    if(_components[i].value()==c){
      _components.erase(i);
      break;
    }
  updateComponents();
}

void Entity::destroy(Entity* e){
  for(auto&& entity : _destroyQueue)
    if(entity==e)
      return;
  _destroyQueue.push(e);
}
void Entity::flushDestroyQueue() {
  while(!_destroyQueue.empty()){
    delete _destroyQueue.back();
    _destroyQueue.pop();
  }
}
