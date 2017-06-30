#include "Context.h"

#include "../containers/Ref.h"
#include "../math/Rand.h"
#include "../streams/CFileStream.h"
#include "../math/Vector.h"
#include "../system/Window.h"
#include "../time/Time.h"

using namespace L;
using namespace Script;

Table<Symbol, Var> Context::_globals;
Table<const TypeDescription*, Var> Context::_typeTables;
StaticStack<128, Var> Context::_stack;
StaticStack<16, uint32_t> Context::_frames;
StaticStack<16, Var> Context::_selves;

static void object(Context& c) {
  Table<Var, Var>& table(c.returnValue().make<Ref<Table<Var, Var>>>().make());
  const uint32_t params(c.localCount());
  for(uint32_t i(1); i<params; i += 2)
    table[c.local(i-1)] = c.local(i);
}
static void applyScope(Var& v, Table<Symbol, uint32_t>& localTable, uint32_t& localIndex) {
  static const Symbol localSymbol("local"), funSymbol("fun"), foreachSymbol("foreach"), setSymbol("set");
  if(v.is<Array<Var>>()) {
    Array<Var>& array(v.as<Array<Var>>());
    if(array[0].is<Symbol>()) {
      const Symbol& sym(array[0].as<Symbol>());
      if(array[1].is<Symbol>() && sym==localSymbol) {
        array[0] = setSymbol;
        localTable[array[1].as<Symbol>()] = localIndex++;
      } else if(sym==foreachSymbol) {
        if(array.size()>=4) // Value only
          localTable[array[1].as<Symbol>()] = localIndex++;
        if(array.size()>=5) // Key value
          localTable[array[2].as<Symbol>()] = localIndex++;
      } else if(sym==funSymbol) // Don't go inside functions
        return;
    }
    for(auto&& e : array)
      applyScope(e, localTable, localIndex);
  } else if(v.is<Symbol>()) {
    if(auto found = localTable.find(v.as<Symbol>()))
      v = Local{*found};
  }
}

CodeFunction Context::read(Stream& stream) {
  static const Symbol doSymbol("do");
  CodeFunction wtr{Array<Var>{doSymbol}};
  Array<Var>& array(wtr.code.as<Array<Var>>());
  Script::Lexer lexer(stream);
  lexer.nextToken();
  while(!stream.end()) {
    array.push();
    if(!read(array.back(), lexer)) { // Read failed
      array.size(1); // Return do nothing function
      return wtr;
    }
  }
  Table<Symbol, uint32_t> localTable;
  uint32_t localIndex(0);
  applyScope(wtr.code, localTable, localIndex);
  wtr.localCount = localIndex;
  return wtr;
}
bool Context::read(Var& v, Lexer& lexer) {
  if(lexer.eos()) {
    L_WARNING("Unexpected end of stream while parsing script: there are uneven () or {}.");
    return false;
  }
  if(lexer.acceptToken("(")) { // It's a list of expressions
    v.make<Array<Var> >();
    int i(0);
    while(!lexer.acceptToken(")"))
      if(lexer.acceptToken("|"))
        v = Array<Var>{v}, i = 1;
      else if(!read(v[i++], lexer))
        return false;
  } else if(lexer.acceptToken("{")) { // It's an object
    v.make<Array<Var> >();
    v[0] = (Function)object;
    int i(1);
    while(!lexer.acceptToken("}"))
      if(!read(v[i++], lexer))
        return false;
  } else if(lexer.acceptToken("'")) {
    if(!read(v, lexer))
      return false;
    if(!v.is<Symbol>()) {
      L_WARNINGF("Expected symbol after ', got %s at line %d", v.type()->name, lexer.line());
      return false;
    }
    const Symbol sym(v.as<Symbol>());
    v.make<RawSymbol>().sym = sym;
  } else if(lexer.acceptToken("!")) {
    if(!read(v, lexer))
      return false;
  } else if(lexer.isToken(")") || lexer.isToken("}")) {
    L_WARNINGF("Unexpected token %s at line %d", lexer.token(), lexer.line());
    return false;
  } else {
    const char* token(lexer.token());
    if(lexer.literal()) v = token; // Character string
    else if(strpbrk(token, "0123456789")) { // Has digits
      if(token[strspn(token, "-0123456789")]=='\0') v = atoi(token); // Integer
      else if(token[strspn(token, "-0123456789.")]=='\0') v = (float)atof(token); // Float
      else v = Symbol(token);
    } else if(lexer.isToken("true")) v = true;
    else if(lexer.isToken("false")) v = false;
    else v = Symbol(token);
    lexer.nextToken();
  }
  return true;
}

Var Context::executeReturn(const Var& code) {
  const Var wtr(executeRef(code));
  _stack.pop();
  return wtr;
}
Var& Context::executeRef(const Var& code) {
  const size_t stackSize(_stack.size());
  execute(code);
  L_ASSERT(stackSize+1);
  return _stack.top();
}
void Context::discardExecute(const Var& code) {
  _stack.pop();
  execute(code);
}
void Context::executeDiscard(const Var& code) {
  const size_t stackSize(_stack.size());
  execute(code);
  L_ASSERT(stackSize+1);
  _stack.pop();
}
void Context::execute(const Var& code, Var* selfOut) {
  static const Symbol selfSymbol("self");
  if(code.is<Array<Var> >()) {
    const Array<Var>& array(code.as<Array<Var>>()); // Get reference of array value
    Var selfIn;
    execute(array[0], &selfIn); // Execute first child of array to get function handle
    const Var& handle(_stack.top());
    if(handle.is<Native>())
      handle.as<Native>()(*this, array);
    else if(handle.is<Function>() || handle.is<Ref<CodeFunction>>()) {
      for(uint32_t i(1); i<array.size(); i++) // For all parameters
        execute(array[i]); // Compute parameter values
      _frames.push(uint32_t(_stack.size()-array.size()+1)); // Save local frame
      if(!selfIn.is<void>()) _selves.push(selfIn);
      if(handle.is<Ref<CodeFunction>>()) {
        const Ref<CodeFunction>& function(handle.as<Ref<CodeFunction>>());
        _stack.size(currentFrame()+function->localCount);
        execute(function->code);
        returnValue() = _stack.top();
      } else if(handle.is<Function>()) // It's a function pointer
        handle.as<Function>()(*this); // Call function
      _stack.size(_frames.top()); // Resize to the previous frame
      _frames.pop();
      if(!selfIn.is<void>()) _selves.pop();
    } else if(array.size()>1 && !handle.is<void>()) {
      Ref<Table<Var, Var>> table;
      if(selfOut) *selfOut = handle;
      if(handle.is<Ref<Table<Var, Var>>>())
        table = handle.as<Ref<Table<Var, Var>>>();
      else
        table = typeTable(handle.type());
      _stack.pop(); // Pop handle
      execute(array[1]); // Push index
      _stack.top() = (*table)[_stack.top()]; // Fetch value in table (replace index)
    } else
      err << "Unable to execute command " << array << "\n";
  } else if(code.is<Local>())
    _stack.push(local(code.as<Local>().i));
  else if(code.is<Symbol>()) // It's a global variable or self
    _stack.push(code.as<Symbol>()==selfSymbol ? currentSelf() : global(code.as<Symbol>()));
  else if(code.is<RawSymbol>()) _stack.push(code.as<RawSymbol>().sym); // Return raw symbol
  else _stack.push(code); // Return raw value
}
Var* Context::reference(const Var& code, Var* src) {
  static const Symbol selfSymbol("self");
  if(code.is<Local>()) // It's a reference to a local variable
    return &local(code.as<Local>().i);
  if(code.is<Symbol>()) // It's a symbol so it's a reference to a global variable or self
    return (code.as<Symbol>()==selfSymbol) ? &currentSelf() : &global(code.as<Symbol>());
  else if(code.is<Array<Var> >()) { // It's an array so it may be a reference to an object field or a
    const Array<Var>& array(code.as<Array<Var> >());
    if(array.size()==2) { // It's a pair
      Var& handle(executeRef(array[0])); // Compute handle
      Ref<Table<Var, Var>> table;
      if(src) *src = handle;
      if(handle.is<Ref<Table<Var, Var>>>()) { // First is a regular table
        table = handle.as<Ref<Table<Var, Var>>>();
      } else if(!handle.is<Ref<CodeFunction>>() // First is not any kind of function or void
                && !handle.is<Native>()
                && !handle.is<Function>()
                && !handle.is<void>()) {
        table = typeTable(handle.type());
      } else if(handle.is<void>())
        L_ERRORF("Trying to index from void");
      else return nullptr;
      execute(array[1]); // Compute index
      Var* wtr(&(*table)[_stack.top()]); // Get pointer to field
      _stack.pop(); // Clean up index
      _stack.pop(); // Clean up handle
      return wtr;
    }
  }
  return nullptr;
}
bool Context::tryExecuteMethod(const Symbol& sym, std::initializer_list<Var> parameters) {
  static Var methodCall(Array<Var>(1, Var(Array<Var>{Symbol("self"), Script::RawSymbol()})));
  static Array<Var>& callArray(methodCall.as<Array<Var>>());
  static Symbol& callSym(callArray[0].as<Array<Var>>()[1].as<Script::RawSymbol>().sym);
  auto it(_self.as<Ref<Table<Var, Var>>>()->find(sym));
  if(it) {
    callSym = sym;
    callArray.size(1);
    for(auto&& p : parameters)
      callArray.push(p);
    _selves.push(_self);
    executeDiscard(methodCall);
    _selves.pop();
    return true;
  }
  return false;
}
Var Context::executeInside(const Var& code) {
  _selves.push(_self);
  execute(code);
  const Var wtr(_stack.top());
  _stack.pop();
  _selves.pop();
  return wtr;
}

Ref<Table<Var, Var>> Context::typeTable(const TypeDescription* td) {
  Var& tt(_typeTables[td]);
  if(!tt.is<Ref<Table<Var, Var>>>())
    tt = ref<Table<Var, Var>>();
  return tt.as<Ref<Table<Var, Var>>>();
}

Context::Context() : _self(ref<Table<Var, Var>>()) {
  L_ONCE;
  _globals[Symbol("fun")] = (Native)([](Context& c, const Array<Var>& a) {
    if(a.size()>1) {
      Ref<CodeFunction>& fun(c._stack.top().make<Ref<CodeFunction>>());
      Table<Symbol, uint32_t> localTable;
      uint32_t localIndex(0);
      fun.make();
      if(a.size()>2 && a[1].is<Array<Var> >()) // There's a list of parameter symbols
        for(auto&& sym : a[1].as<Array<Var> >()) {
          L_ASSERT(sym.is<Symbol>());
          localTable[sym.as<Symbol>()] = localIndex++;
        }
      fun->code = a.back();
      applyScope(fun->code, localTable, localIndex); // The last part is always the code
      fun->localCount = localIndex;
    }
  });
  _globals[Symbol("local")] = (Native)([](Context& c, const Array<Var>& a) {
    L_BREAKPOINT;
  });
  _globals[Symbol("do")] = (Native)([](Context& c, const Array<Var>& a) {
    for(uintptr_t i(1); i<a.size(); i++)
      c.discardExecute(a[i]);
  });
  _globals[Symbol("and")] = (Native)([](Context& c, const Array<Var>& a) {
    for(uintptr_t i(1); i<a.size(); i++) {
      c.discardExecute(a[i]);
      if(!c._stack.top().get<bool>())
        return;
    }
  });
  _globals[Symbol("not")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()==1);
    c.returnValue() = !c.local(0).get<bool>();
  });
  _globals[Symbol("non-null")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()==1);
    c.returnValue() = c.local(0).as<void*>()!=nullptr;
  });
  _globals[Symbol("count")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()==1);
    if(c.local(0).is<Ref<Table<Var, Var>>>())
      c.returnValue() = (int)c.local(0).as<Ref<Table<Var, Var>>>()->count();
    else c.returnValue() = 0;
  });
  _globals[Symbol("or")] = (Native)([](Context& c, const Array<Var>& a) {
    for(uintptr_t i(1); i<a.size(); i++) {
      c.discardExecute(a[i]);
      if(c._stack.top().get<bool>())
        return;
    }
  });
  _globals[Symbol("while")] = (Native)([](Context& c, const Array<Var>& a) {
    L_ASSERT(a.size()==3);
    while(true) {
      c.discardExecute(a[1]);
      if(c._stack.top().get<bool>())
        c.discardExecute(a[2]);
      else return;
    }
  });
  _globals[Symbol("foreach")] = (Native)([](Context& c, const Array<Var>& a) {
    L_ASSERT(a.size()>=4);
    Var *key(nullptr), *value;
    const Var* exec;
    Var* tableVar;
    if(a.size()==4) { // Value only
      value = &c.local(a[1].as<Local>().i);
      tableVar = &c.executeRef(a[2]);
      exec = &a[3];
    } else { // Key value
      key = &c.local(a[1].as<Local>().i);
      value = &c.local(a[2].as<Local>().i);
      tableVar = &c.executeRef(a[3]);
      exec = &a[4];
    }
    for(auto&& slot : *tableVar->as<Ref<Table<Var, Var>>>()) {
      if(key) *key = slot.key();
      *value = slot.value();
      c.discardExecute(*exec);
    }
  });
  _globals[Symbol("if")] = (Native)([](Context& c, const Array<Var>& a) {
    for(uintptr_t i(1); i<a.size()-1; i += 2) {
      c.discardExecute(a[i]);
      if(c._stack.top().get<bool>()) {
        c.discardExecute(a[i+1]);
        return;
      }
    }
    if(!(a.size()&1))
      c.discardExecute(a.back());
  });
  _globals[Symbol("switch")] = (Native)([](Context& c, const Array<Var>& a) {
    if(a.size()>1) {
      const Var& testValue(c.executeRef(a[1])); // Compute test value
      for(uintptr_t i(2); i<a.size()-1; i += 2) {
        const Var& caseValue(c.executeRef(a[i]));
        if(caseValue==testValue) {
          c._stack.pop(); // Pop case value
          c._stack.pop(); // Pop test value
          c.discardExecute(a[i+1]);
          return;
        }
      }
      c._stack.pop(); // Pop test value
      if(a.size()&1)
        c.discardExecute(a.back());
    }
  });
  _globals[Symbol("set")] = (Native)([](Context& c, const Array<Var>& a) {
    if(a.size()==3) {
      c.discardExecute(a[2]);
      *c.reference(a[1]) = c._stack.top();
    }
  });
#define CMP(name,cop)\
  _globals[Symbol(name)] = (Function)([](Context& c) {\
    L_ASSERT(c.localCount()>=2);\
    for(uintptr_t i(1); i<c.localCount(); i++)\
      if(c.local(i-1) cop c.local(i)){\
        c.returnValue() = false;\
        return;\
      }\
    c.returnValue() = true;\
  })
  CMP("=", !=);
  CMP("<>", ==);
  CMP(">", <=);
  CMP("<", >=);
  CMP(">=", <);
  CMP("<=", >);
#undef CMP
  _globals[Symbol("max")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()>=1);
    c.returnValue() = c.local(0);
    for(uintptr_t i(1); i<c.localCount(); i++)
      if(c.local(i)>c.returnValue())
        c.returnValue() = c.local(i);
  });
  _globals[Symbol("min")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()>=1);
    c.returnValue() = c.local(0);
    for(uintptr_t i(1); i<c.localCount(); i++)
      if(c.local(i)<c.returnValue())
        c.returnValue() = c.local(i);
  });
#define SETOP(op)\
  _globals[Symbol(#op)] = (Native)([](Context& c,const Array<Var>& a) {\
    L_ASSERT(a.size()>1);\
    Var* target(c.reference(a[1]));\
    for(uintptr_t i(2);i<a.size();i++){\
      *target op c.executeRef(a[i]);\
      c._stack.pop();\
    }\
  })
  SETOP(+= );
  SETOP(-= );
  SETOP(*= );
  SETOP(/= );
  SETOP(%= );
#undef SETOP
  _globals[Symbol("+")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()>=1);
    c.returnValue() = c.local(0);
    for(uintptr_t i(1); i<c.localCount(); i++)
      c.returnValue() += c.local(i);
  });
  _globals[Symbol("*")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()>=1);
    c.returnValue() = c.local(0);
    for(uintptr_t i(1); i<c.localCount(); i++)
      c.returnValue() *= c.local(i);
  });
  _globals[Symbol("-")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()>=1);
    if(c.localCount()==1)
      c.returnValue() = Var(0) - c.local(0); // TODO: replace with actual neg dynamic operator
    else {
      c.returnValue() = c.local(0);
      for(uintptr_t i(1); i<c.localCount(); i++)
        c.returnValue() -= c.local(i);
    }
  });
  _globals[Symbol("/")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()==2);
    c.returnValue() = c.local(0)/c.local(1);
  });
  _globals[Symbol("%")] = (Function)([](Context& c) {
    L_ASSERT(c.localCount()==2);
    c.returnValue() = c.local(0)%c.local(1);
  });
  _globals[Symbol("print")] = (Function)([](Context& c) {
    for(uintptr_t i(0); i<c.localCount(); i++)
      out << c.local(i);
  });
  _globals[Symbol("break")] = (Function)([](Context& c) {
    L_BREAKPOINT;
  });
  _globals[Symbol("typename")] = (Function)([](Context& c) {
    c.returnValue() = c.local(0).type()->name;
  });
  _globals[Symbol("object")] = (Function)object;
  _globals[Symbol("now")] = (Function)([](Context& c) {
    c.returnValue() = Time::now();
  });
  _globals["rand"] = (Function)([](Context& c) {
    c.returnValue() = Rand::nextFloat();
  });
  _globals[Symbol("button-pressed")] = (Function)([](Context& c) {
    if(c.localCount()) {
      if(c.local(0).is<Symbol>())
        c.returnValue() = Window::isPressed(Window::symbolToButton(c.local(0).as<Symbol>()));
      else if(c.local(0).is<int>())
        c.returnValue() = Window::isPressed((Window::Event::Button)(c.local(0).as<int>()+'0'));
    } else c.returnValue() = false;
  });
  _globals[Symbol("vec")] = (Function)([](Context& c) {
    if(c.localCount()==3)
      c.returnValue() = Vector3f(c.local(0).get<float>(), c.local(1).get<float>(), c.local(2).get<float>());
  });
  _globals[Symbol("color")] = (Function)([](Context& c) {
    Color wtr(Color::white);
    const uint32_t params(min(c.localCount(), 4u));
    for(size_t i(0); i<params; i++) {
      byte b((c.local(i).is<float>()) ? (c.local(i).as<float>()*255) : c.local(i).get<int>());
      switch(i) {
        case 0: wtr.r() = b; break;
        case 1: wtr.g() = b; break;
        case 2: wtr.b() = b; break;
        case 3: wtr.a() = b; break;
      }
    }
    c.returnValue() = wtr;
  });
}
