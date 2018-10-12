#include "ScriptContext.h"

#include "../container/Ref.h"
#include "../engine/Resource.h"
#include "../engine/Resource.inl"
#include "../image/Color.h"
#include "../macros.h"
#include "../math/Rand.h"
#include "../stream/CFileStream.h"
#include "../math/Vector.h"
#include "../system/Device.h"
#include "../system/Window.h"
#include "../time/Time.h"

using namespace L;

Table<const TypeDescription*, Var> ScriptContext::_type_tables;

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
  } else if(object.is<void>())
    error("Trying to index from void");
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
      case CondJump: if(local(ip->a)) ip += intptr_t(ip->bc); break;

      case Add: local(ip->a) += local(ip->b); break;
      case Sub: local(ip->a) -= local(ip->b); break;
      case Mul: local(ip->a) *= local(ip->b); break;
      case Div: local(ip->a) /= local(ip->b); break;
      case Mod: local(ip->a) %= local(ip->b); break;
      case Inv: local(ip->a) = Var(0)-local(ip->a); break; // TODO: replace with actual invert dynamic operator
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
    L_ASSERT(ip+1>=current_script->bytecode.begin() && ip+1<current_script->bytecode.end());
    ip++;
  }
}

Ref<Table<Var, Var>> ScriptContext::type_table(const TypeDescription* td) {
  Var& tt(_type_tables[td]);
  if(!tt.is<Ref<Table<Var, Var>>>())
    tt = ref<Table<Var, Var>>();
  return tt.as<Ref<Table<Var, Var>>>();
}

ScriptContext::ScriptContext(const Var& self) : _self(self), _current_stack_start(0), _current_param_count(0) {
  L_ONCE;
  global("non-null") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = !c.param(0).is<void>() && c.param(0).as<void*>()!=nullptr;
  });
  global("count") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    if(c.param(0).is<Ref<Table<Var, Var>>>())
      c.return_value() = float(c.param(0).as<Ref<Table<Var, Var>>>()->count());
    else c.return_value() = 0;
  });
  global("max") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()>=1);
    c.return_value() = c.param(0);
    for(uintptr_t i(1); i<c.param_count(); i++)
      if(c.param(i)>c.return_value())
        c.return_value() = c.param(i);
  });
  global("min") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()>=1);
    c.return_value() = c.param(0);
    for(uintptr_t i(1); i<c.param_count(); i++)
      if(c.param(i)<c.return_value())
        c.return_value() = c.param(i);
  });
  global("clamp") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==3);
    c.return_value() = clamp(c.param(0), c.param(1), c.param(2));
  });
  global("print") = (ScriptNativeFunction)([](ScriptContext& c) {
    for(uintptr_t i(0); i<c.param_count(); i++)
      out << c.param(i);
  });
  global("break") = (ScriptNativeFunction)([](ScriptContext& c) {
    debugbreak();
  });
  global("typename") = (ScriptNativeFunction)([](ScriptContext& c) {
    c.return_value() = c.param(0).type()->name;
  });
  global("now") = (ScriptNativeFunction)([](ScriptContext& c) {
    c.return_value() = Time::now();
  });
  global("time") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = Time(c.param(0).get<float>()*1000000.f);
  });
  global("rand") = (ScriptNativeFunction)([](ScriptContext& c) {
    c.return_value() = Rand::nextFloat();
  });
  global("button-pressed") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count()) {
      c.return_value() = Device::any_button(Device::symbol_to_button(c.param(0)));
    } else c.return_value() = false;
  });
  global("window-height") = (ScriptNativeFunction)([](ScriptContext& c) { c.return_value() = float(Window::height()); });
  global("window-width") = (ScriptNativeFunction)([](ScriptContext& c) { c.return_value() = float(Window::width()); });
  global("mouse-x") = (ScriptNativeFunction)([](ScriptContext& c) { c.return_value() = float(Window::mouse_x()); });
  global("mouse-y") = (ScriptNativeFunction)([](ScriptContext& c) { c.return_value() = float(Window::mouse_y()); });
  global("vec") = (ScriptNativeFunction)([](ScriptContext& c) {
    const uint32_t param_count(c.param_count());
    Vector3f& vector(c.return_value().make<Vector3f>());
    if(param_count)
      for(uint32_t i(0); i<3; i++)
        vector[i] = c.param(min(param_count-1, i));
    else vector = 0.f;
  });
  global("vec4") = (ScriptNativeFunction)([](ScriptContext& c) {
    const uint32_t param_count(c.param_count());
    Vector4f& vector(c.return_value().make<Vector4f>());
    if(param_count)
      for(uint32_t i(0); i<4; i++)
        vector[i] = c.param(min(param_count-1, i));
    else vector = 0.f;
  });
  global("normalize") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = c.param(0).get<Vector3f>().normalized();
  });
  global("cross") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==2);
    c.return_value() = c.param(0).get<Vector3f>().cross(c.param(1).get<Vector3f>());
  });
  global("length") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = c.param(0).get<Vector3f>().length();
  });
  global("distance") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==2);
    c.return_value() = c.param(0).get<Vector3f>().dist(c.param(1).get<Vector3f>());
  });
  global("dot") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==2);
    c.return_value() = c.param(0).get<Vector3f>().dot(c.param(1).get<Vector3f>());
  });
  global("sqrt") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = sqrt(c.param(0).get<float>());
  });
  global("pow") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==2);
    c.return_value() = powf(c.param(0).get<float>(), c.param(1).get<float>());
  });
  global("sin") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = sinf(c.param(0).get<float>());
  });
  global("cos") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = cosf(c.param(0).get<float>());
  });
  global("tan") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = tanf(c.param(0).get<float>());
  });
  global("lerp") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==3);
    const float a(c.param(0).get<float>()), b(c.param(1).get<float>()), alpha(c.param(2).get<float>());
    c.return_value() = (a*(1.f-alpha)+b*alpha);
  });
  global("abs") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = abs(c.param(0).get<float>());
  });
  global("floor") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==1);
    c.return_value() = floorf(c.param(0).get<float>());
  });
  global("color") = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param(0).is<String>()) {
      c.return_value() = Color(c.param(0).as<String>());
    } else {
      Color& color(c.return_value().make<Color>() = Color::white);
      const uint32_t params(min(c.param_count(), 4u));
      for(uint32_t i(0); i<params; i++)
        color[i] = c.param(i).is<float>() ? (c.param(i).as<float>()*255) : c.param(i).get<int>();
    }
  });
  global("left-pad") = (ScriptNativeFunction)([](ScriptContext& c) {
    L_ASSERT(c.param_count()==3);
    c.return_value() = c.param(0).get<String>();
    String& str(c.return_value().as<String>());
    const uint32_t wanted_size(c.param(1).get<float>());
    const String append(c.param(2).get<String>());
    while(str.size()<wanted_size) {
      str = append + str;
    }
  });
#define L_SCRIPT_ACCESS_METHOD(type,name) type_value(Type<type>::description(), Symbol(#name)) = (ScriptNativeFunction)([](ScriptContext& c) {L_ASSERT(c.param_count()==0 && c.current_self().is<type>()); c.return_value() = c.current_self().as<type>().name();})
  L_SCRIPT_ACCESS_METHOD(Vector3f, x);
  L_SCRIPT_ACCESS_METHOD(Vector3f, y);
  L_SCRIPT_ACCESS_METHOD(Vector3f, z);
}
