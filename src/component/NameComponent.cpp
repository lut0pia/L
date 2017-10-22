#include "NameComponent.h"

using namespace L;

Table<Symbol, NameComponent*> NameComponent::_components;

Map<Symbol, Var> NameComponent::pack() const {
  Map<Symbol, Var> data;
  data["name"] = _name;
  return data;
}
void NameComponent::unpack(const Map<Symbol, Var>& data) {
  Symbol symbol;
  unpack_item(data, "name", symbol);
  name(symbol);
}
void NameComponent::script_registration() {
  L_COMPONENT_BIND(NameComponent, "name");
  L_COMPONENT_METHOD(NameComponent, "set", 1, name(c.local(0).get<Symbol>()));
  L_COMPONENT_METHOD(NameComponent, "get", 0, name());
}

NameComponent* NameComponent::find(const Symbol& name) {
  if(NameComponent** name_component = _components.find(name))
    return *name_component;
  else
    return nullptr;
}
