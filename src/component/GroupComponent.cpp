#include "GroupComponent.h"

using namespace L;

static Handle<GroupComponent> group_context;

GroupComponent::~GroupComponent() {
  for(Handle<Entity> linked_entity : _entities) {
    Entity::destroy(linked_entity);
  }
}
void GroupComponent::update() {
  L_ASSERT(!group_context.is_valid());

  // Check if the level script has loaded or changed since last execution
  const ScriptFunction* level_script = _level_script.try_load();
  if(level_script && _level_script.slot()->value != _level_script_value) {
    reload_level_script();
    _level_script_value = _level_script.slot()->value;
  }
}
void GroupComponent::script_registration() {
  L_COMPONENT_BIND(GroupComponent, "group");
  L_SCRIPT_METHOD(GroupComponent, "link", 1, link(c.param(0).get<Handle<Entity>>()));
  L_SCRIPT_METHOD(GroupComponent, "unlink", 1, unlink(c.param(0).get<Handle<Entity>>()));
  L_SCRIPT_METHOD(GroupComponent, "unlink_all", 0, unlink_all());
  L_SCRIPT_METHOD(GroupComponent, "level_script", 1, level_script(c.param(0).get<String>()));
  L_SCRIPT_METHOD(GroupComponent, "reload_level_script", 0, reload_level_script());
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

void GroupComponent::reload_level_script() {
  const ScriptFunction* level_script = _level_script.try_load();
  if(level_script != nullptr) {
    // Destroy all linked entities
    for(Handle<Entity> linked_entity : _entities) {
      Entity::destroy(linked_entity);
    }

    group_context = handle();
    ScriptContext().execute(ref<ScriptFunction>(*level_script));
    group_context = nullptr;
  }
}

void GroupComponent::notify_entity_created(Handle<Entity> entity) {
  if(group_context) {
    group_context->link(entity);
  }
}
