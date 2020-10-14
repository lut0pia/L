#include "NameComponent.h"

using namespace L;

Table<Symbol, NameComponent*> NameComponent::_components;

void NameComponent::script_registration() {
  L_COMPONENT_BIND(NameComponent, "name");
  L_SCRIPT_METHOD(NameComponent, "set", 1, name(c.param(0).get<Symbol>()));
  L_SCRIPT_METHOD(NameComponent, "get", 0, name());
}

NameComponent* NameComponent::find(const Symbol& name) {
  if(NameComponent** name_component = _components.find(name))
    return *name_component;
  else
    return nullptr;
}
