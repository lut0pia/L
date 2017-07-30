#include "HierarchyComponent.h"

using namespace L;

void HierarchyComponent::lateUpdate() {
  if(_parent) {
    _transform->position(_parent->position() + _translation);
    _transform->rotation(_parent->rotation() * _rotation);
  }
}
void HierarchyComponent::updateComponents() {
  _transform = entity()->requireComponent<Transform>();
}