#include "HierarchyComponent.h"

using namespace L;

void HierarchyComponent::late_update() {
  if(_parent) {
    _transform->position(_parent->position() + _parent->rotation().rotate(_translation));
    _transform->rotation(_parent->rotation() * _rotation);
  }
}
void HierarchyComponent::update_components() {
  _transform = entity()->requireComponent<Transform>();
}
void HierarchyComponent::script_registration() {
  L_COMPONENT_BIND(HierarchyComponent, "hierarchy");
  L_COMPONENT_METHOD(HierarchyComponent, "parent", 1, parent(c.local(0).get<Transform*>()));
  L_COMPONENT_METHOD(HierarchyComponent, "translation", 1, translation(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(HierarchyComponent, "rotation", 2, rotation(c.local(0).get<Vector3f>(), c.local(1).get<float>()));
  L_COMPONENT_METHOD(HierarchyComponent, "rotate", 2, rotate(c.local(0).get<Vector3f>(), c.local(1).get<float>()));
  L_COMPONENT_METHOD(HierarchyComponent, "rotate-absolute", 2, rotate_absolute(c.local(0).get<Vector3f>(), c.local(1).get<float>()));
}
