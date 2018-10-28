#include "ScriptContext.h"

#include "../container/Ref.h"
#include "../engine/Resource.h"
#include "../engine/Resource.inl"

using namespace L;

Table<const TypeDescription*, Var> ScriptContext::_type_tables;

ScriptContext::ScriptContext(const Var& self)
  : _self(self), _current_stack_start(0), _current_param_count(0) {
}

struct ObjectIterator {
  Table<Var, Var>::Iterator begin, end;
  // Default ctor necessary for Type
  inline ObjectIterator() : begin(nullptr), end(nullptr) {}
  inline ObjectIterator(const Ref<Table<Var, Var>>& table)
    : begin(table ? table->begin() : nullptr), end(table ? table->end() : nullptr) {
  }
  inline void iterate(Var& key, Var& value) {
    const Table<Var, Var>::Slot& slot(*begin);
    key = slot.key();
    value = slot.value();
    ++begin;
  }
  inline bool has_ended() {
    return !(begin != end);
  }
};

static Ref<Table<Var, Var>> get_table(const Var& object) {
  Ref<Table<Var, Var>> table;
  if(object.is<Ref<Table<Var, Var>>>()) { // First is a regular table
    table = object.as<Ref<Table<Var, Var>>>();
  } else if(!object.is<ScriptFunction>() // First is not any kind of function or void
    && !object.is<ScriptNativeFunction>()
    && !object.is<void>()) {
    table = ScriptContext::type_table(object.type());
  } else {
    warning("Trying to index from non-indexable type: %s", object.type()->name);
  }
  return table;
}
static inline void get_item(const Var& object, const Var& index, Var& res) {
  if(Ref<Table<Var, Var>> table = get_table(object)) {
    res = (*table)[index];
  }
}
static inline void set_item(Var& object, const Var& index, const Var& value) {
  if(Ref<Table<Var, Var>> table = get_table(object)) {
    (*table)[index] = value;
  }
}
bool ScriptContext::try_execute_method(const Symbol& sym, std::initializer_list<Var> parameters) {
  if(_self.is<Ref<Table<Var, Var>>>()) {
    if(const Var* method = _self.as<Ref<Table<Var, Var>>>()->find(sym)) {
      if(method->is<ScriptFunction>()) {
        execute(method->as<ScriptFunction>(), parameters);
        return true;
      }
    }
  }
  return false;
}
Var ScriptContext::execute(const ScriptFunction& function, const Var* params, size_t param_count) {
  L_SCOPE_MARKER("Script execution");
  L_ASSERT(_frames.empty());
  _stack.size(1<<12);
  _frames.push();
  _frames.back().script = function.script;
  _frames.back().stack_start = _current_stack_start = 0;
  _frames.back().param_count = _current_param_count = param_count;

  current_self() = _self;

  { // Put parameters on the stack
    for(uintptr_t i(0); i<param_count; i++) {
      param(i) = params[i];
    }
  }

  Ref<Script> current_script(function.script);
  const ScriptInstruction* ip(function.script->bytecode.begin()+function.offset);

  while(true) {
    L_ASSERT(ip>=current_script->bytecode.begin() && ip<current_script->bytecode.end());
    switch(ip->opcode) {
      case CopyLocal: local(ip->a) = local(ip->b); break;
      case LoadConst: local(ip->a) = current_script->constants[ip->b]; break;
      case LoadBool: local(ip->a) = (ip->b!=0); break;
      case LoadInt: local(ip->a) = float(ip->bc); break;
      case LoadGlobal: local(ip->a) = current_script->globals[ip->b].value(); break;
      case StoreGlobal: current_script->globals[ip->a].value() = local(ip->b); break;
      case LoadFun: local(ip->a) = ScriptFunction {current_script, uintptr_t(ip->bc)}; break;

      case MakeObject: local(ip->a) = ref<Table<Var, Var>>(); break;
      case GetItem: get_item(local(ip->a), local(ip->b), local(ip->c)); break;
      case SetItem: set_item(local(ip->a), local(ip->b), local(ip->c)); break;

      case MakeIterator: local(ip->a) = ObjectIterator(get_table(local(ip->b))); break;
      case Iterate: local(ip->c).as<ObjectIterator>().iterate(local(ip->a), local(ip->b)); break;
      case IterEndJump: if(local(ip->a).as<ObjectIterator>().has_ended()) ip += intptr_t(ip->bc); break;

      case Jump: ip += intptr_t(ip->bc); break;
      case CondJump: if(local(ip->a).get<bool>()) ip += intptr_t(ip->bc); break;
      case CondNotJump: if(!local(ip->a).get<bool>()) ip += intptr_t(ip->bc); break;

      case Add: local(ip->a) += local(ip->b); break;
      case Sub: local(ip->a) -= local(ip->b); break;
      case Mul: local(ip->a) *= local(ip->b); break;
      case Div: local(ip->a) /= local(ip->b); break;
      case Mod: local(ip->a) %= local(ip->b); break;
      case Inv: local(ip->a).invert(); break;
      case Not: local(ip->a) = !local(ip->a).get<bool>(); break;

      case LessThan: local(ip->a) = (local(ip->b) < local(ip->c)); break;
      case LessEqual: local(ip->a) = (local(ip->b) <= local(ip->c)); break;
      case Equal: local(ip->a) = (local(ip->b) == local(ip->c)); break;

      case Call:
      {
        const Var& new_func(local(ip->a));

        _current_stack_start += ip->a;
        _current_param_count = ip->b;

        if(new_func.is<ScriptNativeFunction>()) {
          return_value().as<ScriptNativeFunction>()(*this);
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = _frames.back().param_count;
        } else if(new_func.is<ScriptFunction>()) {
          const ScriptFunction& script_function(new_func.as<ScriptFunction>());
          L_ASSERT(script_function.offset<script_function.script->bytecode.size());

          _frames.back().ip = ip; // Save instruction pointer
          _frames.push();
          _frames.back().stack_start = _current_stack_start;
          _frames.back().param_count = _current_param_count;

          current_script = _frames.back().script = script_function.script;
          ip = current_script->bytecode.begin()+script_function.offset;
          continue; // Avoid ip increment
        }
        break;
      }
      case Return:
        _frames.pop();
        if(_frames.empty()) {
          return _stack[0];
        } else {
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = _frames.back().param_count;

          current_script = _frames.back().script;
          ip = _frames.back().ip;
        }
        break;
    }
    ip++;
  }
}

Ref<Table<Var, Var>> ScriptContext::type_table(const TypeDescription* td) {
  Var& tt(_type_tables[td]);
  if(!tt.is<Ref<Table<Var, Var>>>())
    tt = ref<Table<Var, Var>>();
  return tt.as<Ref<Table<Var, Var>>>();
}
