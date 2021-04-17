#include "HierarchyComponent.h"

using namespace L;

void HierarchyComponent::late_update() {
  if(_parent) {
    _transform->position(_parent->position() + _parent->rotation().rotate(_translation));
    _transform->rotation(_parent->rotation() * _rotation);
  }
}
void HierarchyComponent::update_components() {
  _transform = entity()->require_component<Transform>();
}
void HierarchyComponent::script_registration() {
  L_COMPONENT_BIND(HierarchyComponent, "hierarchy");
  L_SCRIPT_METHOD(HierarchyComponent, "parent", 1, parent(c.param(0).get<Handle<Transform>>()));
  L_SCRIPT_METHOD(HierarchyComponent, "translation", 1, translation(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(HierarchyComponent, "translate", 1, translate(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(HierarchyComponent, "rotation", 2, rotation(c.param(0).get<Quatf>()));
  L_SCRIPT_METHOD(HierarchyComponent, "rotate", 1, rotate(c.param(0).get<Quatf>()));
  L_SCRIPT_METHOD(HierarchyComponent, "rotate_absolute", 1, rotate_absolute(c.param(0).get<Quatf>()));
}
