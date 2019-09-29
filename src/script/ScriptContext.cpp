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
      if(method->is<Ref<ScriptFunction>>()) {
        execute(method->as<Ref<ScriptFunction>>(), parameters);
        return true;
      }
    }
  }
  return false;
}
Var ScriptContext::execute(const Ref<ScriptFunction>& function, const Var* params, size_t param_count) {
  L_SCOPE_MARKER("Script execution");
  L_ASSERT(_frames.empty());
  _stack.size(1 << 12);
  _frames.push();
  _frames.back().script = function->script;
  _frames.back().stack_start = _current_stack_start = 0;
  _frames.back().param_count = param_count;
  _current_param_count = uint32_t(param_count);

  current_self() = _self;

  { // Put parameters on the stack
    for(uintptr_t i(0); i < param_count; i++) {
      param(i) = params[i];
    }
  }

  Ref<ScriptFunction> current_function = function;
  Ref<Script> current_script = function->script;
  const ScriptInstruction* ip = function->script->bytecode.begin() + function->offset;

  while(true) {
    L_ASSERT(ip >= current_script->bytecode.begin() && ip < current_script->bytecode.end());
    switch(ip->opcode) {
      case CopyLocal: local(ip->a) = local(ip->bc8.b); break;
      case LoadConst: local(ip->a) = current_script->constants[ip->bcu16]; break;
      case LoadGlobal: local(ip->a) = current_script->globals[ip->bcu16].value(); break;
      case StoreGlobal: current_script->globals[ip->bcu16].value() = local(ip->a); break;
      case LoadFun: local(ip->a) = ref<ScriptFunction>(ScriptFunction {current_script, uintptr_t(ip->bc16)}); break;

      case LoadOuter:
      {
        Ref<ScriptOuter> outer = current_function->outers[ip->bc8.b];
        local(ip->a) = (outer->offset > 0) ? _stack[outer->offset] : outer->value;
        break;
      }
      case StoreOuter:
      {
        Ref<ScriptOuter> outer = current_function->outers[ip->a];
        ((outer->offset > 0) ? _stack[outer->offset] : outer->value) = local(ip->bc8.b);
        break;
      }
      case CaptLocal:
      {
        L_ASSERT(local(ip->a).is<Ref<ScriptFunction>>());
        Ref<ScriptOuter> outer = ref<ScriptOuter>(ScriptOuter {ip->bc8.b + _current_stack_start});
        local(ip->a).as<Ref<ScriptFunction>>()->outers.push(outer);
        _outers.push(outer);
        break;
      }
      case CaptOuter: local(ip->a).as<Ref<ScriptFunction>>()->outers.push(current_function->outers[ip->bc8.b]); break;

      case MakeObject: local(ip->a) = ref<Table<Var, Var>>(); break;
      case GetItem: get_item(local(ip->a), local(ip->bc8.b), local(ip->bc8.c)); break;
      case SetItem: set_item(local(ip->a), local(ip->bc8.b), local(ip->bc8.c)); break;

      case MakeIterator: local(ip->a) = ObjectIterator(get_table(local(ip->bc8.b))); break;
      case Iterate: local(ip->bc8.c).as<ObjectIterator>().iterate(local(ip->a), local(ip->bc8.b)); break;
      case IterEndJump: if(local(ip->a).as<ObjectIterator>().has_ended()) ip += intptr_t(ip->bc16); break;

      case Jump: ip += intptr_t(ip->bc16); break;
      case CondJump: if(local(ip->a).get<bool>()) ip += intptr_t(ip->bc16); break;
      case CondNotJump: if(!local(ip->a).get<bool>()) ip += intptr_t(ip->bc16); break;

      case Add: local(ip->a) += local(ip->bc8.b); break;
      case Sub: local(ip->a) -= local(ip->bc8.b); break;
      case Mul: local(ip->a) *= local(ip->bc8.b); break;
      case Div: local(ip->a) /= local(ip->bc8.b); break;
      case Mod: local(ip->a) %= local(ip->bc8.b); break;
      case Inv: local(ip->a).invert(); break;
      case Not: local(ip->a) = !local(ip->a).get<bool>(); break;

      case LessThan: local(ip->a) = (local(ip->bc8.b) < local(ip->bc8.c)); break;
      case LessEqual: local(ip->a) = (local(ip->bc8.b) <= local(ip->bc8.c)); break;
      case Equal: local(ip->a) = (local(ip->bc8.b) == local(ip->bc8.c)); break;

      case Call:
      {
        const Var& new_func(local(ip->a));

        _current_stack_start += ip->a;
        _current_param_count = ip->bc8.b;

        if(new_func.is<ScriptNativeFunction>()) {
          return_value().as<ScriptNativeFunction>()(*this);
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = uint32_t(_frames.back().param_count);
        } else if(new_func.is<Ref<ScriptFunction>>()) {
          const Ref<ScriptFunction>& script_function(new_func.as<Ref<ScriptFunction>>());
          L_ASSERT(script_function->offset < script_function->script->bytecode.size());

          _frames.back().ip = ip; // Save instruction pointer
          _frames.push();
          _frames.back().stack_start = _current_stack_start;
          _frames.back().param_count = _current_param_count;

          current_function = script_function;
          current_script = _frames.back().script = script_function->script;
          ip = current_script->bytecode.begin() + script_function->offset;
          continue; // Avoid ip increment
        } else {
          warning("Trying to call non-callable type: %s", new_func.type()->name);
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = uint32_t(_frames.back().param_count);
        }
        break;
      }
      case Return:
        // Unlink outers
        for(uintptr_t i = 0; i < _outers.size(); i++) {
          if(_outers[i]->offset >= _current_stack_start) {
            _outers[i]->value = _stack[_outers[i]->offset];
            _outers[i]->offset = 0;
            _outers.erase_fast(i);
            i -= 1;
          }
        }

        _frames.pop();
        if(_frames.empty()) {
          return _stack[0];
        } else {
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = uint32_t(_frames.back().param_count);

          current_script = _frames.back().script;
          ip = _frames.back().ip;
        }
        break;

        // Optimization opcodes
      case LoadBool: local(ip->a) = (ip->bc8.b != 0); break;
      case LoadInt: local(ip->a) = float(ip->bc16); break;
      default:
        error("Unhandled script instruction");
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
