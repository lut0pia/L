#include "Context.h"

#include "../containers/Ref.h"
#include "../math/Rand.h"
#include "../streams/FileStream.h"
#include "../math/Vector.h"
#include "../system/Window.h"
#include "../time/Time.h"

using namespace L;
using namespace Script;

Table<Symbol, Var> Context::_globals;
Table<const TypeDescription*, Var> Context::_typeTables;

static Var object(Context& c) {
  Ref<Table<Var, Var> > wtr(ref<Table<Var, Var>>());
  Table<Var, Var>& table(*wtr);
  const uint32_t params(c.localCount());
  for(uint32_t i(1); i<params; i += 2)
    table[c.local(i-1)] = c.local(i);
  return wtr;
}
static void applyScope(Var& v, Table<Symbol, uint32_t>& localTable, uint32_t& localIndex) {
  static Symbol localSymbol("local"), funSymbol("fun"), foreachSymbol("foreach");
  if(v.is<Array<Var>>()) {
    Array<Var>& array(v.as<Array<Var>>());
    if(array[0].is<Symbol>()) {
      const Symbol& sym(array[0].as<Symbol>());
      if(array[1].is<Symbol>() && sym==localSymbol) {
        array[0] = Symbol("set");
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
    const Symbol& sym(v.as<Symbol>());
    auto* it(localTable.find(sym));
    if(it) v = Local{it->value()};
  }
}

void Context::read(Stream& stream) {
  Script::Lexer lexer(stream);
  Table<Symbol, uint32_t> localTable;
  uint32_t localIndex(0);
  lexer.nextToken();
  while(!stream.end()) {
    Var v;
    read(v, lexer);
    applyScope(v, localTable, localIndex);
    _stack.size(localIndex);
    execute(v);
  }
  _stack.size(0);
}
void Context::read(Var& v, Lexer& lexer) {
  if(lexer.eos())
    L_ERROR("Unexpected end of stream while parsing script: there are uneven () or {}.");
  if(lexer.acceptToken("(")) { // It's a list of expressions
    v.make<Array<Var> >();
    int i(0);
    while(!lexer.acceptToken(")"))
      if(lexer.acceptToken("|"))
        v = Array<Var>{v}, i = 1;
      else read(v[i++], lexer);
  } else if(lexer.acceptToken("{")) { // It's an object
    v.make<Array<Var> >();
    v[0] = (Function)object;
    int i(1);
    while(!lexer.acceptToken("}"))
      read(v[i++], lexer);
  } else if(lexer.acceptToken("'")) {
    read(v.make<Quote>().var, lexer);
  } else if(lexer.acceptToken("!")) {
    read(v, lexer);
    v = execute(v);
  } else if(lexer.isToken(")") || lexer.isToken("}")) {
    L_ERRORF("Unexpected token %s at line %d", lexer.token(), lexer.line());
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
}

Var Context::execute(const Var& code, Var* selfOut) {
  static const Symbol selfSymbol("self");
  if(code.is<Array<Var> >()) {
    const Array<Var>& array(code.as<Array<Var> >()); // Get reference of array value
    Var selfIn;
    const Var& handle(execute(array[0], &selfIn)); // Execute first child of array to get function handle
    if(handle.is<Native>())
      return handle.as<Native>()(*this, array);
    else if(handle.is<Function>() || handle.is<Ref<CodeFunction> >()) {
      Var wtr;
      uint32_t frame(_stack.size());
      for(uint32_t i(1); i<array.size(); i++) // For all parameters
        _stack.push(execute(array[i])); // Compute parameter values
      _frames.push(frame); // Save local frame
      if(!selfIn.is<void>()) _selves.push(selfIn);
      if(handle.is<Ref<CodeFunction> >()) {
        const Ref<CodeFunction>& function(handle.as<Ref<CodeFunction> >());
        _stack.size(currentFrame()+function->localCount);
        wtr = execute(function->code);
      } else if(handle.is<Function>()) // It's a function pointer
        wtr = handle.as<Function>()(*this); // Call function
      _stack.size(_frames.top()); // Resize to the previous frame
      _frames.pop();
      if(!selfIn.is<void>()) _selves.pop();
      return wtr;
    } else if(array.size()>1) {
      Ref<Table<Var, Var>> table;
      if(selfOut) *selfOut = handle;
      if(handle.is<Ref<Table<Var, Var>>>())
        table = handle.as<Ref<Table<Var, Var>>>();
      else
        table = typeTable(handle.type());
      return (*table)[execute(array[1])];
    }
  } else if(code.is<Local>())
    return local(code.as<Local>().i);
  else if(code.is<Symbol>()) // It's a global variable or self
    return (code.as<Symbol>()==selfSymbol) ? currentSelf() : global(code.as<Symbol>());
  else if(code.is<Quote>()) return code.as<Quote>().var; // Return raw data
  return code;
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
      Var first(execute(array[0]));
      Ref<Table<Var, Var>> table;
      if(src) *src = first;
      if(first.is<Ref<Table<Var, Var>>>()) { // First is a regular table
        table = first.as<Ref<Table<Var, Var>>>();
      } else if(!first.is<Ref<CodeFunction>>() // First is not any kind of function or void
                && !first.is<Native>()
                && !first.is<Function>()
                && !first.is<void>()) {
        table = typeTable(first.type());
      } else if(first.is<void>())
        L_ERRORF("Trying to index from void");
      else return nullptr;
      return &(*table)[execute(array[1])]; // Compute index and return pointer to field
    }
  }
  return nullptr;
}
bool Context::tryExecuteMethod(const Symbol& sym, std::initializer_list<Var> parameters) {
  static Var methodCall(Array<Var>(1, Var(Array<Var>{Symbol("self"), Script::Quote{Symbol()}})));
  static Array<Var>& callArray(methodCall.as<Array<Var>>());
  static Symbol& callSym(callArray[0].as<Array<Var>>()[1].as<Script::Quote>().var.as<Symbol>());
  auto it(_self.as<Ref<Table<Var, Var>>>()->find(sym));
  if(it) {
    callSym = sym;
    callArray.size(1);
    for(auto&& p : parameters)
      callArray.push(p);
    execute(methodCall);
    return true;
  }
  return false;
}

Ref<Table<Var, Var>> Context::typeTable(const TypeDescription* td) {
  Var& tt(_typeTables[td]);
  if(!tt.is<Ref<Table<Var, Var>>>())
    tt = ref<Table<Var, Var>>();
  return tt.as<Ref<Table<Var, Var>>>();
}

Context::Context() : _self(ref<Table<Var, Var>>()) {
  L_ONCE;
  _globals[Symbol("fun")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    if(a.size()>1) {
      Ref<CodeFunction> wtr;
      Table<Symbol, uint32_t> localTable;
      uint32_t localIndex(0);
      wtr.make();
      if(a.size()>2 && a[1].is<Array<Var> >()) // There's a list of parameter symbols
        for(auto&& sym : a[1].as<Array<Var> >()) {
          L_ASSERT(sym.is<Symbol>());
          localTable[sym.as<Symbol>()] = localIndex++;
        }
      wtr->code = a.back();
      applyScope(wtr->code, localTable, localIndex); // The last part is always the code
      wtr->localCount = localIndex;
      return wtr;
    }
    return 0;
  });
  _globals[Symbol("local")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    L_BREAKPOINT;
    return 0;
  });
  _globals[Symbol("do")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    for(uintptr_t i(1); i<a.size()-1; i++)
      c.execute(a[i]);
    return c.execute(a.back());
  });
  _globals[Symbol("and")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    for(uintptr_t i(1); i<a.size(); i++)
      if(!c.execute(a[i]).get<bool>())
        return false;
    return true;
  });
  _globals[Symbol("not")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()==1);
    return !c.local(0).get<bool>();
  });
  _globals[Symbol("non-null")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()==1);
    return c.local(0).as<void*>()!=nullptr;
  });
  _globals[Symbol("count")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()==1);
    if(c.local(0).is<Ref<Table<Var, Var>>>())
      return (int)c.local(0).as<Ref<Table<Var, Var>>>()->count();
    return 0;
  });
  _globals[Symbol("or")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    for(uintptr_t i(1); i<a.size(); i++)
      if(c.execute(a[i]).get<bool>())
        return true;
    return false;
  });
  _globals[Symbol("while")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    Var wtr;
    while(c.execute(a[1]).get<bool>())
      wtr = c.execute(a[2]);
    return wtr;
  });
  _globals[Symbol("foreach")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    L_ASSERT(a.size()>=4);
    Var *key(nullptr), *value;
    const Var* exec;
    Var tableVar;
    if(a.size()==4) { // Value only
      value = &c.local(a[1].as<Local>().i);
      tableVar = c.execute(a[2]);
      exec = &a[3];
    } else { // Key value
      key = &c.local(a[1].as<Local>().i);
      value = &c.local(a[2].as<Local>().i);
      tableVar = c.execute(a[3]);
      exec = &a[4];
    }
    for(auto&& slot : *tableVar.as<Ref<Table<Var, Var>>>()) {
      if(key) *key = slot.key();
      *value = slot.value();
      c.execute(*exec);
    }
    return Var();
  });
  _globals[Symbol("if")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    for(uintptr_t i(1); i<a.size()-1; i += 2)
      if(c.execute(a[i]).get<bool>())
        return c.execute(a[i+1]);
    if(!(a.size()&1))
      return c.execute(a.back());
    else return 0;
  });
  _globals[Symbol("switch")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    if(a.size()>1) {
      const Var v(c.execute(a[1]));
      for(uintptr_t i(2); i<a.size()-1; i += 2)
        if(c.execute(a[i])==v)
          return c.execute(a[i+1]);
      if(a.size()&1)
        return c.execute(a.back());
    }
    return 0;
  });
  _globals[Symbol("set")] = (Native)([](Context& c, const Array<Var>& a)->Var {
    if(a.size()==3)
      return *c.reference(a[1]) = c.execute(a[2]);
    else return 0;
  });
#define CMP(name,cop)\
  _globals[Symbol(name)] = (Function)([](Context& c)->Var {\
    L_ASSERT(c.localCount()>=2);\
    for(uintptr_t i(1); i<c.localCount(); i++)\
      if(c.local(i-1) cop c.local(i))\
        return false;\
    return true;\
  })
  CMP("=", !=);
  CMP("<>", ==);
  CMP(">", <=);
  CMP("<", >=);
  CMP(">=", <);
  CMP("<=", >);
#undef CMP
  _globals[Symbol("max")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()>=1);
    Var wtr(c.local(0));
    for(uintptr_t i(1); i<c.localCount(); i++)
      if(c.local(i)>wtr)
        wtr = c.local(i);
    return wtr;
  });
  _globals[Symbol("min")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()>=1);
    Var wtr(c.local(0));
    for(uintptr_t i(1); i<c.localCount(); i++)
      if(c.local(i)<wtr)
        wtr = c.local(i);
    return wtr;
  });
#define SETOP(op)\
  _globals[Symbol(#op)] = (Native)([](Context& c,const Array<Var>& a)->Var {\
    L_ASSERT(a.size()>1);\
    Var* target(c.reference(a[1]));\
    for(uintptr_t i(2);i<a.size();i++)\
      *target op c.execute(a[i]);\
    return 0;\
  })
  SETOP(+= );
  SETOP(-= );
  SETOP(*= );
  SETOP(/= );
  SETOP(%= );
#undef SETOP
  _globals[Symbol("+")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()>=1);
    Var wtr(c.local(0));
    for(uintptr_t i(1); i<c.localCount(); i++)
      wtr += c.local(i);
    return wtr;
  });
  _globals[Symbol("*")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()>=1);
    Var wtr(c.local(0));
    for(uintptr_t i(1); i<c.localCount(); i++)
      wtr *= c.local(i);
    return wtr;
  });
  _globals[Symbol("-")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()>=1);
    if(c.localCount()==1)
      return Var(0) - c.local(0); // TODO: replace with actual neg dynamic operator
    else {
      Var wtr(c.local(0));
      for(uintptr_t i(1); i<c.localCount(); i++)
        wtr -= c.local(i);
      return wtr;
    }
  });
  _globals[Symbol("/")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()==2);
    return c.local(0)/c.local(1);
  });
  _globals[Symbol("%")] = (Function)([](Context& c)->Var {
    L_ASSERT(c.localCount()==2);
    return c.local(0)%c.local(1);
  });
  _globals[Symbol("print")] = (Function)([](Context& c)->Var {
    for(uintptr_t i(0); i<c.localCount(); i++)
      out << c.local(i);
    return 0;
  });
  _globals[Symbol("break")] = (Function)([](Context& c)->Var {
    L_BREAKPOINT;
    return 0;
  });
  _globals[Symbol("typename")] = (Function)([](Context& c)->Var {
    return c.local(0).type()->name;
  });
  _globals[Symbol("object")] = (Function)object;
  _globals[Symbol("now")] = (Function)([](Context& c)->Var {
    return Time::now();
  });
  _globals["rand"] = (Function)([](Context& c)->Var {
    return Rand::nextFloat();
  });
  _globals[Symbol("button-pressed")] = (Function)([](Context& c)->Var {
    if(c.localCount()) {
      if(c.local(0).is<Symbol>())
        return Window::isPressed(Window::symbolToButton(c.local(0).as<Symbol>()));
      else if(c.local(0).is<int>())
        return Window::isPressed((Window::Event::Button)(c.local(0).as<int>()+'0'));
    }
    return false;
  });
  _globals[Symbol("vec")] = (Function)([](Context& c)->Var {
    if(c.localCount()==3)
      return Vector3f(c.local(0).get<float>(), c.local(1).get<float>(), c.local(2).get<float>());
    return Vector3f();
  });
  _globals[Symbol("color")] = (Function)([](Context& c)->Var {
    Color wtr(Color::white);
    uint32_t params(min(c.localCount(), 4u));
    for(size_t i(0); i<params; i++) {
      byte b((c.local(i).is<float>()) ? (c.local(i).as<float>()*255) : c.local(i).get<int>());
      switch(i) {
        case 0: wtr.r() = b; break;
        case 1: wtr.g() = b; break;
        case 2: wtr.b() = b; break;
        case 3: wtr.a() = b; break;
      }
    }
    return wtr;
  });
}
