#include "Entity.h"

#include "Component.h"

using namespace L;

IterablePool<Entity> Entity::_pool;
Array<Entity*> Entity::_destroy_queue;

Entity::Entity(const Entity* other) {
  for(auto p : other->_components){
    p.value() = (Component*)p.key()->cpy(p.value());
    p.value()->entity(this);
    _components.push(p);
    p.value()->updateComponents();
  }
}
Entity::~Entity() {
  // Components remove themselves from entity on destruction
  _destroyed = true;
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
  if(!_destroyed)
    updateComponents();
}

void Entity::destroy(Entity* e) {
  if(_destroy_queue.find(e)==-1)
    _destroy_queue.push(e);
}
void Entity::flush_destroy_queue() {
  for(Entity* entity : _destroy_queue)
    delete entity;
  _destroy_queue.clear();
}
void Entity::clear() {
  Array<Entity*> entities(_pool.objects());
  for(Entity* entity : entities)
    delete entity;
}
