#pragma once

#include "script.h"
#include "../container/Array.h"
#include "../dynamic/Variable.h"
#include "../container/Ref.h"
#include "../container/Table.h"
#include "../text/Symbol.h"

namespace L {
  class ScriptContext {
  private:
    Array<Var> _stack;
    struct Frame {
      Ref<ScriptFunction> function;
      Ref<Script> script;
      const ScriptInstruction* ip;
      uintptr_t stack_start;
      uint32_t param_count;
    };
    Array<Frame> _frames;
    Array<Ref<ScriptOuter>> _outers;
    Var _self;
    uintptr_t _current_stack_start = 0;
    uint32_t _current_param_count = 0;

  public:
    inline ScriptContext(const Var& self = ref<Table<Var, Var>>()) : _self(self) {}

    inline Var& local(uintptr_t i) { return _stack[i+_current_stack_start]; }
    inline Var& return_value() { return local(0); }
    inline Var& current_self() { return local(1); }
    inline Var& param(uintptr_t i) { return local(i+2); } // Parameters start after return value and self

    inline Var& self() { return _self; }
    inline Table<Var, Var>& self_table() { return *_self.as<Ref<Table<Var, Var>>>(); }
    inline uint32_t local_count() const { return uint32_t(_stack.size()-_current_stack_start); }
    inline uint32_t param_count() const { return _current_param_count; }

    bool try_execute_method(const Symbol&, std::initializer_list<Var> = {});
    Var execute(const Ref<ScriptFunction>& function, std::initializer_list<Var> il) {
      return execute(function, il.begin(), il.size());
    }
    Var execute(const Ref<ScriptFunction>& function, const Var* params = nullptr, size_t param_count = 0);

    static Var& type_value(const TypeDescription* type, const Var& index);
    static ScriptGetItemFunction& type_get_item(const TypeDescription* type);
    static ScriptSetItemFunction& type_set_item(const TypeDescription* type);
  };
}
