#pragma once

#include "Component.h"
#include "../text/Symbol.h"

namespace L {
  class NameComponent : public Component {
    L_COMPONENT(NameComponent)
  protected:
    static Table<Symbol, NameComponent*> _components;
    Symbol _name;
  public:
    inline NameComponent() { name(ntos<16>(Rand::next(), sizeof(uint64_t)*2)); }
    inline ~NameComponent() { _components.remove(_name); }

    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    inline void name(const Symbol& name) {
      _components.remove(_name);
      if(_components.find(name)) {
        L_ERROR("Name already in use");
      } else {
        _name = name;
        _components[_name] = this;
      }
    }
    inline const Symbol& name() const { return _name; }

    static NameComponent* find(const Symbol&);
  };
}
