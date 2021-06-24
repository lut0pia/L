#include "Entity.h"

#include "Component.h"
#include "NameComponent.h"
#include "GroupComponent.h"
#include "../stream/CFileStream.h"

using namespace L;

static IterablePool<Entity> entity_pool;
static Array<Handle<Entity>> entity_destroy_queue;

Entity::~Entity() {
  _destroyed = true;
  for(auto& component : _components) {
    component.key()->del(component.value().pointer());
  }
}

void Entity::update_components() {
  for(auto& p : _components) {
    if(Component* component = (Component*)p.value().pointer()) {
      component->update_components();
    }
  }
}
void Entity::remove(Handle<Component> c) {
  for(uint32_t i(0); i < _components.size(); i++)
    if(_components[i].value() == c) {
      _components.erase(i);
      break;
    }
  if(!_destroyed) {
    update_components();
  }
}

Handle<Entity> Entity::create() {
  Entity* entity_ptr = new(entity_pool.allocate())Entity();
  Handle<Entity> entity = entity_ptr->handle();
  GroupComponent::notify_entity_created(entity);
  return entity;
}
Handle<Entity> Entity::copy(Handle<Entity> other) {
  Handle<Entity> entity = create();
  Entity* entity_ptr = entity;
  for(const auto& p : other->_components) {
    Component* new_component = (Component*)p.key()->cpy(p.value().pointer());
    new_component->_entity = entity;
    entity_ptr->_components.push(p.key(), *(Handle<Component>*)&new_component->generic_handle());
  }
  entity_ptr->update_components();
  return entity;
}

void Entity::destroy(Handle<Entity> e) {
  if(entity_destroy_queue.find(e) == nullptr) {
    entity_destroy_queue.push(e);
  }
}
void Entity::flush_destroy_queue() {
  for(Handle<Entity> entity : entity_destroy_queue) {
    if(Entity* entity_ptr = entity) {
      entity_ptr->~Entity();
      entity_pool.deallocate(entity_ptr);
    }
  }
  entity_destroy_queue.clear();
}
void Entity::clear() {
  Array<Entity*> entities = entity_pool.objects();
  for(Entity* entity : entities) {
    entity->~Entity();
    entity_pool.deallocate(entity);
  }
}
