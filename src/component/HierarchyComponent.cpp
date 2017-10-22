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
