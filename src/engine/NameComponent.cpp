#include "NameComponent.h"

using namespace L;

Table<Symbol, NameComponent*> NameComponent::_components;

NameComponent* NameComponent::find(const Symbol& name) {
  if(NameComponent** name_component = _components.find(name))
    return *name_component;
  else
    return nullptr;
}