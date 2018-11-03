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
    static Table<const TypeDescription*, Var> _type_tables;
    Array<Var> _stack;
    struct Frame {
      Ref<Script> script;
      const ScriptInstruction* ip;
      uintptr_t stack_start;
      uintptr_t param_count;
    };
    Array<Frame> _frames;
    uintptr_t _current_stack_start;
    uint32_t _current_param_count;
    Var _self;

  public:
    ScriptContext(const Var& self = ref<Table<Var, Var>>());

    inline Var& local(uintptr_t i) { return _stack[i+_current_stack_start]; }
    inline Var& return_value() { return local(0); }
    inline Var& current_self() { return local(1); }
    inline Var& param(uintptr_t i) { return local(i+2); } // Parameters start after return value and self

    inline Table<Var, Var>& self_table() { return *_self.as<Ref<Table<Var, Var>>>(); }
    inline uint32_t local_count() const { return uint32_t(_stack.size()-_current_stack_start); }
    inline uint32_t param_count() const { return _current_param_count; }

    bool try_execute_method(const Symbol&, std::initializer_list<Var> = {});
    Var execute(const ScriptFunction& function, std::initializer_list<Var> il) {
      return execute(function, il.begin(), il.size());
    }
    Var execute(const ScriptFunction& function, const Var* params = nullptr, size_t param_count = 0);

    static inline Var& global(Symbol s) { return ScriptGlobal(s).value(); }
    static Ref<Table<Var, Var>> type_table(const TypeDescription*);
    static inline Var& type_value(const TypeDescription* td, const Var& k) { return (*type_table(td))[k]; }
  };
}
