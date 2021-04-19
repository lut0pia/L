#include "ScriptContext.h"

#include <stdarg.h>

#include "../container/Ref.h"
#include "../engine/Resource.h"
#include "../engine/Resource.inl"

using namespace L;

static Table<const TypeDescription*, Table<Var, Var>> type_tables;
static Table<const TypeDescription*, ScriptGetItemFunction> type_get_items;
static Table<const TypeDescription*, ScriptSetItemFunction> type_set_items;

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

static Var make_iterator(const ScriptContext& c, const Var& object) {
  if(const Ref<Array<Var>>* array = object.try_as<Ref<Array<Var>>>()) {
    return ArrayIterator(*array);
  } else if(const Ref<Table<Var, Var>>* table = object.try_as<Ref<Table<Var, Var>>>()) {
    return TableIterator(*table);
  } else {
    c.warning("Trying to create iterator for non-iterable type: %s", object.type()->name);
    return Var();
  }
}
static void iterate(const ScriptContext& c, Var& object, Var& key, Var& value) {
  if(TableIterator* table_iterator = object.try_as<TableIterator>()) {
    table_iterator->iterate(key, value);
  } else if(ArrayIterator* array_iterator = object.try_as<ArrayIterator>()) {
    array_iterator->iterate(key, value);
  } else {
    c.warning("Trying to iterate non-iterator type: %s", object.type()->name);
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
static inline void get_item(const ScriptContext& c, const Var& object, const Var& index, Var& value) {
  ScriptGetItemFunction* func = type_get_items.find(object.type());
  if(!func || !(*func)(c, object, index, value)) {
    if(Var* type_value = type_tables[object.type()].find(index)) {
      value = *type_value;
    } else {
      c.warning("Couldn't get item from object of type %s with index: %s", object.type()->name, (const char*)to_string(index));
    }
  }
}
static inline void set_item(const ScriptContext& c, Var& object, const Var& index, const Var& value) {
  ScriptSetItemFunction* func = type_set_items.find(object.type());
  if(!func || !(*func)(c, object, index, value)) {
    c.warning("Couldn't set item of object of type %s with index: %s", object.type()->name, (const char*)to_string(index));
  }
}
static inline void push_item(const ScriptContext& c, Var& object, const Var& value) {
  if(Ref<Array<Var>>* array = object.try_as<Ref<Array<Var>>>()) {
    (*array)->push(value);
  } else {
    c.warning("Trying to push to non-pushable type: %s", object.type()->name);
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

  const uintptr_t start_frame_count = _frames.size();
  const uintptr_t start_stack_start = _current_stack_start;
  const uint32_t start_param_count = _current_param_count;

  // Increase stack start to keep previous locals untouched
  _current_stack_start += _current_param_count + 2;

  _stack.size(_current_stack_start + 256);
  _frames.push();
  _frames.back().function = function;
  _frames.back().ip = function->script->bytecode.begin() + function->offset;
  _frames.back().stack_start = _current_stack_start;
  _frames.back().param_count = _current_param_count = uint32_t(param_count);

  current_self() = _self;

  { // Put parameters on the stack
    for(uintptr_t i(0); i < param_count; i++) {
      param(i) = params[i];
    }
  }

  Ref<ScriptFunction> current_function = function;
  Ref<Script> current_script = function->script;

  while(true) {
    const ScriptInstruction ip = *_frames.back().ip;
    switch(ip.opcode) {
      case CopyLocal: local(ip.a) = local(ip.bc8.b); break;
      case LoadConst: local(ip.a) = current_script->constants[ip.bcu16]; break;
      case LoadGlobal: local(ip.a) = current_script->globals[ip.bcu16].value(); break;
      case StoreGlobal: current_script->globals[ip.bcu16] = local(ip.a); break;
      case LoadFun: local(ip.a) = ref<ScriptFunction>(ScriptFunction {current_script, uintptr_t(ip.bc16)}); break;

      case LoadOuter:
      {
        Ref<ScriptOuter> outer = current_function->outers[ip.bc8.b];
        local(ip.a) = (outer->offset > 0) ? _stack[outer->offset] : outer->value;
        break;
      }
      case StoreOuter:
      {
        Ref<ScriptOuter> outer = current_function->outers[ip.a];
        ((outer->offset > 0) ? _stack[outer->offset] : outer->value) = local(ip.bc8.b);
        break;
      }
      case CaptLocal:
      {
        L_ASSERT(local(ip.a).is<Ref<ScriptFunction>>());
        Ref<ScriptOuter> outer = ref<ScriptOuter>(ScriptOuter {ip.bc8.b + _current_stack_start});
        local(ip.a).as<Ref<ScriptFunction>>()->outers.push(outer);
        _outers.push(outer);
        break;
      }
      case CaptOuter: local(ip.a).as<Ref<ScriptFunction>>()->outers.push(current_function->outers[ip.bc8.b]); break;

      case MakeObject: local(ip.a) = ref<Table<Var, Var>>(); break;
      case MakeArray: local(ip.a) = ref<Array<Var>>(); break;
      case GetItem: get_item(*this, local(ip.a), local(ip.bc8.b), local(ip.bc8.c)); break;
      case SetItem: set_item(*this, local(ip.a), local(ip.bc8.b), local(ip.bc8.c)); break;
      case PushItem: push_item(*this, local(ip.a), local(ip.bc8.b)); break;

      case MakeIterator: local(ip.a) = make_iterator(*this, local(ip.bc8.b)); break;
      case Iterate: iterate(*this, local(ip.bc8.c), local(ip.a), local(ip.bc8.b)); break;
      case IterEndJump: if(iterator_has_ended(local(ip.a))) _frames.back().ip += intptr_t(ip.bc16); break;

      case Jump: _frames.back().ip += intptr_t(ip.bc16); break;
      case CondJump: if(local(ip.a).get<bool>()) _frames.back().ip += intptr_t(ip.bc16); break;
      case CondNotJump: if(!local(ip.a).get<bool>()) _frames.back().ip += intptr_t(ip.bc16); break;

      case Add: local(ip.a) += local(ip.bc8.b); break;
      case Sub: local(ip.a) -= local(ip.bc8.b); break;
      case Mul: local(ip.a) *= local(ip.bc8.b); break;
      case Div: local(ip.a) /= local(ip.bc8.b); break;
      case Mod: local(ip.a) %= local(ip.bc8.b); break;
      case Inv: local(ip.a).invert(); break;
      case Not: local(ip.a) = !local(ip.a).get<bool>(); break;

      case LessThan: local(ip.a) = (local(ip.bc8.b) < local(ip.bc8.c)); break;
      case LessEqual: local(ip.a) = (local(ip.bc8.b) <= local(ip.bc8.c)); break;
      case Equal: local(ip.a) = (local(ip.bc8.b) == local(ip.bc8.c)); break;

      case Call:
      {
        _current_stack_start += ip.a;
        _current_param_count = ip.bc8.b;
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

          _frames.push();
          _frames.back().stack_start = _current_stack_start;
          _frames.back().param_count = _current_param_count;
          _frames.back().function = current_function = script_function;
          current_script = current_function->script;
          _frames.back().ip = current_script->bytecode.begin() + current_function->offset;
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
        if(_frames.size() == start_frame_count) {
          Var value = _stack[_current_stack_start];
          _current_stack_start = start_stack_start;
          _current_param_count = start_param_count;
          _stack.size(_current_stack_start + 256);
          return value;
        } else {
          _current_stack_start = _frames.back().stack_start;
          _current_param_count = _frames.back().param_count;
          _stack.size(_current_stack_start + 256);

          current_function = _frames.back().function;
          current_script = current_function->script;
        }
        break;

        // Optimization opcodes
      case LoadBool: local(ip.a) = (ip.bc8.b != 0); break;
      case LoadInt: local(ip.a) = float(ip.bc16); break;
      case GetItemConst: get_item(*this, local(ip.a), current_script->constants[ip.bc8.b], local(ip.bc8.c)); break;
      case SetItemConst: set_item(*this, local(ip.a), current_script->constants[ip.bc8.b], local(ip.bc8.c)); break;
      default:
        error("Unhandled script instruction");
        break;
    }
    _frames.back().ip++;
  }
}

void ScriptContext::warning(const char* msg, ...) const {
  va_list args;
  va_start(args, msg);
  vwarning(msg, args);
  va_end(args);
  print_callstack();
}
void ScriptContext::print_callstack() const {
#if !L_RLS
  log("Script callstack:");
  for(intptr_t i = _frames.size() - 1; i >= 0; i--) {
    const Frame& frame = _frames[i];
    Ref<Script> script = frame.function->script;
    uintptr_t ins_index = frame.ip - script->bytecode.begin();
    if(ins_index < script->bytecode_line.size()) {
      uintptr_t line = script->bytecode_line[ins_index];
      log(" %s:%03d: %s", script->source_id.begin(), line, script->source_lines[line - 1].begin());
    } else {
      log(" %s:???", script->source_id.begin());
    }
  }
#endif
}

Var& ScriptContext::type_value(const TypeDescription* type, const Var& index) {
  return type_tables[type][index];
}
ScriptGetItemFunction& ScriptContext::type_get_item(const TypeDescription* type) {
  return type_get_items[type];
}
ScriptSetItemFunction& ScriptContext::type_set_item(const TypeDescription* type) {
  return type_set_items[type];
}
