#include "GroupComponent.h"

using namespace L;

static GroupComponent* group_context = nullptr;

GroupComponent::~GroupComponent() {
  for(Handle<Entity> linked_entity : _entities) {
    Entity::destroy(linked_entity);
  }
}
void GroupComponent::update() {
  L_ASSERT(group_context == nullptr);
  // Check if the level script has loaded or changed since last execution
  const ScriptFunction* level_script = _level_script.try_load();
  if(level_script && _level_script.slot()->value != _level_script_value) {
    // Destroy all linked entities
    for(Handle<Entity> linked_entity : _entities) {
      Entity::destroy(linked_entity);
    }

    group_context = this;
    ScriptContext().execute(ref<ScriptFunction>(*level_script));
    group_context = nullptr;

    _level_script_value = _level_script.slot()->value;
  }
}
void GroupComponent::script_registration() {
  L_COMPONENT_BIND(GroupComponent, "group");
  L_SCRIPT_METHOD(GroupComponent, "link", 1, link(c.param(0).get<Handle<Entity>>()));
  L_SCRIPT_METHOD(GroupComponent, "unlink", 1, unlink(c.param(0).get<Handle<Entity>>()));
  L_SCRIPT_METHOD(GroupComponent, "unlink_all", 1, unlink_all());
  L_SCRIPT_METHOD(GroupComponent, "level_script", 1, level_script(c.param(0).get<String>()));
  ScriptGlobal("group_entity_create") = (ScriptNativeFunction)([](ScriptContext& c) {
    c.return_value() = entity_create();
  });
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

Handle<Entity> GroupComponent::entity_create() {
  Handle<Entity> entity = Entity::create();

  if(group_context) {
    group_context->link(entity);
  } else {
    warning("Creating group entity but no group context was set");
  }

  return entity;
}
