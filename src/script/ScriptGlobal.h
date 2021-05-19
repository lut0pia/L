#pragma once

#include "../container/Array.h"
#include "../dynamic/Variable.h"
#include "../text/Symbol.h"

namespace L {
  class ScriptGlobal {
  public:
    struct Slot {
      Symbol name;
      Var value;
    };

  protected:
    Slot* _slot;

  public:
    constexpr ScriptGlobal() : _slot(nullptr) {}
    ScriptGlobal(const Symbol& name);

    inline const Symbol& name() const { return _slot->name; }
    inline Var& value() { return _slot->value; }
    inline Var& operator=(const Var& value) { return _slot->value = value; }

    static Array<Symbol> get_all_names();

    friend Stream& operator<=(Stream&, const ScriptGlobal&);
    friend Stream& operator>=(Stream&, ScriptGlobal&);
  };

  Stream& operator<=(Stream&, const ScriptGlobal&);
  Stream& operator>=(Stream&, ScriptGlobal&);
}
