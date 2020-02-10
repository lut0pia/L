#include "GroupComponent.h"

using namespace L;

GroupComponent::~GroupComponent() {
  for(Handle<Entity> linked_entity : _entities) {
    Entity::destroy(linked_entity);
  }
}
void GroupComponent::update() {
  // Check if the level script has loaded or changed since last execution
  if(_level_script.is_loaded() && _level_script_time < _level_script.slot()->mtime) {
    ScriptContext context;
    Var group_component_entity_var = context.execute(ref<ScriptFunction>(*_level_script));
    if(Handle<Entity>* entity = group_component_entity_var.try_as<Handle<Entity>>()) {
      if(Entity* entity_ptr = *entity) {
        if(GroupComponent* group_component = entity_ptr->get_component<GroupComponent>()) {
          // Destroy currently linked entities
          for(Handle<Entity> linked_entity : _entities) {
            Entity::destroy(linked_entity);
          }
          _entities = group_component->entities(); // Copy other's linked entities
          group_component->unlink_all();
          Entity::destroy(*entity); // Destroy other group entity
        } else {
          warning("Level script '%s' did not output an entity with a group component", _level_script.slot()->id);
        }
      } else {
        warning("Level script '%s' did not output a live entity", _level_script.slot()->id);
      }
    } else {
      warning("Level script '%s' did not output an entity", _level_script.slot()->id);
    }
    _level_script_time = _level_script.slot()->mtime;
  }
}
void GroupComponent::script_registration() {
  L_COMPONENT_BIND(GroupComponent, "group");
  L_SCRIPT_METHOD(GroupComponent, "link", 1, link(c.param(0).get<Handle<Entity>>()));
  L_SCRIPT_METHOD(GroupComponent, "unlink", 1, unlink(c.param(0).get<Handle<Entity>>()));
  L_SCRIPT_METHOD(GroupComponent, "unlink_all", 1, unlink_all());
  L_SCRIPT_METHOD(GroupComponent, "level_script", 1, level_script(c.param(0).get<String>()));
}

void GroupComponent::link(Handle<Entity> entity) {
  _entities.push(entity);
}
void GroupComponent::unlink(Handle<Entity> entity) {
  if(const Handle<Entity>* found = _entities.find(entity)) {
    _entities.erase_fast(found - _entities.begin());
  }
}
void GroupComponent::unlink_all() {
  _entities.clear();
}
