#include "ScriptContext.h"

#include "../container/Ref.h"
#include "../engine/Resource.h"
#include "../engine/Resource.inl"

using namespace L;

Table<const TypeDescription*, Var> ScriptContext::_type_tables;

class TableIterator {
private:
  Ref<Table<Var, Var>> _table;
  Table<Var, Var>::Iterator _begin, _end;
public:
  inline TableIterator() : _begin(nullptr), _end(nullptr) {}
  inline TableIterator(const Ref<Table<Var, Var>>& table)
    : _table(table), _begin(table->begin()), _end(table->end()) {
  }
  inline void iterate(Var& key, Var& value) {
    const Table<Var, Var>::Slot& slot(*_begin);
    key = slot.key();
    value = slot.value();
    ++_begin;
  }
  inline bool has_ended() { return !(_begin != _end); }
};

class ArrayIterator {
private:
  Ref<Array<Var>> _array;
  Var* _begin;
  Var* _cur;
  Var* _end;
public:
  inline ArrayIterator() : _begin(nullptr), _cur(nullptr), _end(nullptr) {}
  inline ArrayIterator(const Ref<Array<Var>>& array)
    : _array(array), _begin(array->begin()), _cur(_begin), _end(array->end()) {
  }
  inline void iterate(Var& key, Var& value) {
    key = float(_cur - _begin);
    value = *_cur;
    ++_cur;
  }
  inline bool has_ended() const { return _cur == _end; }
};

static Ref<Table<Var, Var>> get_table(const Var& object) {
  Ref<Table<Var, Var>> table;
  if(object.is<Ref<Table<Var, Var>>>()) { // First is a regular table
    table = object.as<Ref<Table<Var, Var>>>();
  } else if(!object.is<ScriptFunction>() // First is not any kind of function or void
    && !object.is<ScriptNativeFunction>()
    && !object.is<void>()) {
    table = ScriptContext::type_table(object.type());
  }
  return table;
}
static Var make_iterator(const Var& object) {
  if(const Ref<Array<Var>>* array = object.try_as<Ref<Array<Var>>>()) {
    return ArrayIterator(*array);
  } else if(Ref<Table<Var, Var>> table = get_table(object)) {
    return TableIterator(table);
  } else {
    warning("Trying to create iterator for non-iterable type: %s", object.type()->name);
    return Var();
  }
}
static void iterate(Var& object, Var& key, Var& value) {
  if(TableIterator* table_iterator = object.try_as<TableIterator>()) {
    table_iterator->iterate(key, value);
  } else if(ArrayIterator* array_iterator = object.try_as<ArrayIterator>()) {
    array_iterator->iterate(key, value);
  } else {
    warning("Trying to iterate non-iterator type: %s", object.type()->name);
  }
}
static bool iterator_has_ended(Var& object) {
  if(TableIterator* table_iterator = object.try_as<TableIterator>()) {
    return table_iterator->has_ended();
  } else if(ArrayIterator* array_iterator = object.try_as<ArrayIterator>()) {
    return array_iterator->has_ended();
  } else {
    return true;
  }
}
static inline void get_item(const Var& object, const Var& index, Var& res) {
  const Ref<Array<Var>>* array = object.try_as<Ref<Array<Var>>>();
  if(array && index.is<float>()) {
    const int int_index = index.get<int>();
    if(int_index >= 0 && size_t(int_index) < (*array)->size()) {
      res = (**array)[int_index];
    } else {
      warning("Trying to index out-of-bounds");
    }
  } else if(Ref<Table<Var, Var>> table = get_table(object)) {
    res = (*table)[index];
  } else {
    warning("Trying to index from non-indexable type: %s", object.type()->name);
  }
}
static inline void set_item(Var& object, const Var& index, const Var& value) {
  if(Ref<Array<Var>>* array = object.try_as<Ref<Array<Var>>>()) {
    const int int_index = index.get<int>();
    if(int_index >= 0 && size_t(int_index) < (*array)->size()) {
      (**array)[int_index] = value;
    } else {
      warning("Trying to index out-of-bounds");
    }
  } else if(Ref<Table<Var, Var>> table = get_table(object)) {
    (*table)[index] = value;
  } else {
    warning("Trying to index from non-indexable type: %s", object.type()->name);
  }
}
static inline void push_item(Var& object, const Var& value) {
  if(Ref<Array<Var>>* array = object.try_as<Ref<Array<Var>>>()) {
    (*array)->push(value);
  } else {
    warning("Trying to push to non-pushable type: %s", object.type()->name);
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
  L_ASSERT(_stack.empty() && _frames.empty());
  _stack.size(256);
  _frames.push();
  _frames.back().function = function;
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
      case MakeArray: local(ip->a) = ref<Array<Var>>(); break;
      case GetItem: get_item(local(ip->a), local(ip->bc8.b), local(ip->bc8.c)); break;
      case SetItem: set_item(local(ip->a), local(ip->bc8.b), local(ip->bc8.c)); break;
      case PushItem: push_item(local(ip->a), local(ip->bc8.b)); break;

      case MakeIterator: local(ip->a) = make_iterator(local(ip->bc8.b)); break;
      case Iterate: iterate(local(ip->bc8.c), local(ip->a), local(ip->bc8.b)); break;
      case IterEndJump: if(iterator_has_ended(local(ip->a))) ip += intptr_t(ip->bc16); break;

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
        _current_stack_start += ip->a;
        _current_param_count = ip->bc8.b;
        _stack.size(_current_stack_start + 256);

        const Var& new_func(local(0));
        if(new_func.is<ScriptNativeFunction>()) {
          return_value().as<ScriptNativeFunction>()(*this);
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = uint32_t(_frames.back().param_count);
          _stack.size(_current_stack_start + 256);
        } else if(new_func.is<Ref<ScriptFunction>>()) {
          const Ref<ScriptFunction>& script_function(new_func.as<Ref<ScriptFunction>>());
          L_ASSERT(script_function->offset < script_function->script->bytecode.size());

          _frames.back().ip = ip; // Save instruction pointer
          _frames.push();
          _frames.back().stack_start = _current_stack_start;
          _frames.back().param_count = _current_param_count;
          _frames.back().function = current_function = script_function;
          _frames.back().script = current_script = script_function->script;

          ip = current_script->bytecode.begin() + script_function->offset;
          continue; // Avoid ip increment
        } else {
          warning("Trying to call non-callable type: %s", new_func.type()->name);
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = uint32_t(_frames.back().param_count);
          _stack.size(_current_stack_start + 256);
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
          Var value = _stack[0];
          _stack.clear();
          return value;
        } else {
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = uint32_t(_frames.back().param_count);
          _stack.size(_current_stack_start + 256);

          current_function = _frames.back().function;
          current_script = _frames.back().script;
          ip = _frames.back().ip;
        }
        break;

        // Optimization opcodes
      case LoadBool: local(ip->a) = (ip->bc8.b != 0); break;
      case LoadInt: local(ip->a) = float(ip->bc16); break;
      case GetItemConst: get_item(local(ip->a), current_script->constants[ip->bc8.b], local(ip->bc8.c)); break;
      case SetItemConst: set_item(local(ip->a), current_script->constants[ip->bc8.b], local(ip->bc8.c)); break;
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
