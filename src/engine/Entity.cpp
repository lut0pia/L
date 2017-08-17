#include "Entity.h"

#include "Component.h"
#include "NameComponent.h"
#include "../stream/CFileStream.h"

using namespace L;

IterablePool<Entity> Entity::_pool;
Array<Entity*> Entity::_destroy_queue;

void Entity::set_component_entity(Component* c) {
  c->_entity = this;
}

Entity::Entity(const Entity* other) : Entity() {
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

void Entity::save(const char* path) {
  CFileStream file(path, "w");
  for(Entity* entity : _pool.objects()) {
    if(entity->_persistent) {
      file << "-\n";
      file < *entity;
    }
  }
  file << ".\n";
}
void Entity::load(const char* path) {
  CFileStream file(path, "r");
  while(true) {
    if(strcmp(file.word(), "-")) // No more entities
      break;
    Entity* entity(new Entity());
    file > *entity;
  }
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

Stream& L::operator<(Stream& s, const Entity& e) {
  for(const auto& component : e._components) {
    s << "-\n";
    s < component.key()->name < *component.value();
  }
  s << ".\n";
  return s;
}
Stream& L::operator>(Stream& s, Entity& e) {
  e._persistent = true;
  while(true) {
    if(strcmp(s.word(), "-")) // No more components
      break;
    Symbol component_type_name;
    s > component_type_name;
    if(const TypeDescription** component_type = types.find(component_type_name)) {
      Component* component((Component*)(*component_type)->ctrnew());
      e.set_component_entity(component);
      e._components.push(*component_type, component);
      s > *component;
    } else {
      L_ERRORF("Unknown component type name %s", component_type_name);
    }
  }
  e.updateComponents();
}