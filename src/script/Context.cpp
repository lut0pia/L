#include "Context.h"

#include "../containers/Ref.h"
#include "../math/Rand.h"
#include "../streams/FileStream.h"
#include "../math/Vector.h"
#include "../system/Window.h"
#include "../time/Time.h"

using namespace L;
using namespace Script;

Table<Symbol,Var> Context::_globals;

void Context::read(Stream& stream) {
  Script::Lexer lexer(stream);
  Var v;
  lexer.nextToken();
  while(!stream.end()) {
    read(v,lexer);
    execute(v);
  }
}
void Context::read(Var& v,Lexer& lexer) {
  if(lexer.acceptToken("(")) { // It's a list of expressions
    v.make<Array<Var> >();
    int i(0);
    while(!lexer.acceptToken(")"))
      read(v[i++],lexer);
  } else if(lexer.acceptToken("'")) {
    read(v.make<Quote>().var,lexer);
  } else if(lexer.acceptToken("!")) {
    read(v,lexer);
    v = execute(v);
  } else {
    const char* token(lexer.token());
    if(lexer.literal()) v = token; // Character string
    else if(strpbrk(token,"0123456789")){ // Has digits
      if(token[strspn(token,"-0123456789")]=='\0') v = atoi(token); // Integer
      else if(token[strspn(token,"-0123456789.")]=='\0') v = (float)atof(token); // Float
      else v = fnv1a(token);
    } else if(lexer.isToken("true")) v = true;
    else if(lexer.isToken("false")) v = false;
    else v = fnv1a(token);
    lexer.nextToken();
  }
}

Var& Context::variable(Symbol sym) {
  // Search symbol in locals and sub-locals
  for(uintptr_t i(0); i<_stack.size(); i++)
    if(_stack[i].key()==sym)
      return _stack[i].value();
  // Search symbol in globals
  return _globals[sym];
}
Var& Context::local(Symbol sym){
  _stack.push(SymbolVar(sym));
  return _stack.top().value();
}
Var Context::execute(const Var& code) {
  if(Var* ref = reference(code)) return *ref;
  else if(code.is<Array<Var> >()) { // Function call
    const Array<Var>& array(code.as<Array<Var> >()); // Get reference of array value
    const Var& handle(execute(array[0])); // Execute first child of array to get function handle
    if(handle.is<Native>())
      return handle.as<Native>()(*this,array);
    else if(handle.is<Function>() || handle.is<Ref<CodeFunction> >()) {
      Var wtr;
      size_t frame(_stack.size()); // Save local frame
      SymbolVar* stack(&_stack.top()+1);
      for(uint32_t i(1); i<array.size(); i++) { // For all parameters
        Symbol sym;
        if(handle.is<Ref<CodeFunction> >() && handle.as<Ref<CodeFunction> >()->parameters.size()>=i)
          sym = handle.as<Ref<CodeFunction> >()->parameters[i-1];
        else sym = FNV1A("");
        _stack.push(sym,execute(array[i])); // Compute parameter values
      }
      if(handle.is<Ref<CodeFunction> >())
        wtr = execute(handle.as<Ref<CodeFunction> >()->code);
      else if(handle.is<Function>()) // It's a function pointer
        wtr = handle.as<Function>()(stack,array.size()-1); // Call function
      _stack.size(frame); // Resize to the previous frame
      return wtr;
    } else if(array.size()==3){ // May be a binary operator
      const Var& middle(execute(array[1]));
      if(middle.is<Binary>())
        return middle.as<Binary>()(handle,execute(array[2]));
    }
  } else if(code.is<Quote>()) return code.as<Quote>().var; // Return raw data
  return code;
}
Var* Context::reference(const Var& code) {
  if(code.is<Symbol>()) // It's a symbol so it's a simple reference to a variable
    return &variable(code.as<Symbol>());
  else if(code.is<Array<Var> >()){ // It's an array so it may be a reference to an object field or a
    const Array<Var>& array(code.as<Array<Var> >());
    if(array.size()==2){ // It's a pair
      if(Var* first = reference(array[0])){ // The first is a reference
        if(first->is<Ref<Map<Var,Var> > >()) // It's an object
          return &(*first->as<Ref<Map<Var,Var> > >())[execute(array[1])]; // Compute index and return pointer to field
        else if(first->is<Array<Var> >()) // It's an array
          return &first->as<Array<Var> >()[execute(array[1]).get<int>()]; // Compute index and return pointer to element
      }
    }
  }
  return nullptr;
}

Context::Context(){
  L_ONCE;
  // Local allows to define local variables without overriding more global variables
  _globals[FNV1A("local")] = (Native)([](Context& c,const Array<Var>& a)->Var {
    if(a.size()>1 && a[1].is<Symbol>()){
      if(a.size()>2)
        c.local(a[1].as<Symbol>()) = c.execute(a[2]);
      else c.local(a[1].as<Symbol>());
    }
    return 0;
  });
  _globals[FNV1A("do")] = (Native)([](Context& c,const Array<Var>& a)->Var {
    for(uintptr_t i(1); i<a.size()-1; i++)
      c.execute(a[i]);
    return c.execute(a.back());
  });
  _globals[FNV1A("while")] = (Native)([](Context& c,const Array<Var>& a)->Var {
    Var wtr;
    while(c.execute(a[1]).get<bool>())
      wtr = c.execute(a[2]);
    return wtr;
  });
  _globals[FNV1A("if")] = (Native)([](Context& c,const Array<Var>& a)->Var {
    if(a.size()>2) {
      if(c.execute(a[1]).get<bool>())
        return c.execute(a[2]);
      else if(a.size()>3)
        return c.execute(a[3]);
    }
    return 0;
  });
  _globals[FNV1A("set")] = (Native)([](Context& c,const Array<Var>& a)->Var {
    Var* target;
    if(a.size()==3 && (target = c.reference(a[1])))
      *target = c.execute(a[2]);
    return 0;
  });
  _globals[FNV1A("fun")] = (Native)([](Context& c,const Array<Var>& a)->Var {
    if(a.size()>1){
      Ref<CodeFunction> wtr;
      wtr.make();
      if(a.size()>2 && a[1].is<Array<Var> >()) // There's a list of parameter symbols
        for(auto&& sym : a[1].as<Array<Var> >())
          if(sym.is<Symbol>())
            wtr->parameters.push(sym.as<Symbol>());
      wtr->code = a.back(); // The last part is always the code
      return wtr;
    }
    return 0;
  });
  _globals[FNV1A("=")] = (Binary)([](const Var& a,const Var& b)->Var {return a==b; });
  _globals[FNV1A("<>")] = (Binary)([](const Var& a,const Var& b)->Var {return a!=b; });
  _globals[FNV1A(">")] = (Binary)([](const Var& a,const Var& b)->Var {return a>b; });
  _globals[FNV1A("<")] = (Binary)([](const Var& a,const Var& b)->Var {return a<b; });
  _globals[FNV1A(">=")] = (Binary)([](const Var& a,const Var& b)->Var {return a>=b; });
  _globals[FNV1A("<=")] = (Binary)([](const Var& a,const Var& b)->Var {return a<=b; });
  _globals[FNV1A("+")] = (Binary)([](const Var& a,const Var& b)->Var {return a+b; });
  _globals[FNV1A("-")] = (Binary)([](const Var& a,const Var& b)->Var {return a-b; });
  _globals[FNV1A("*")] = (Binary)([](const Var& a,const Var& b)->Var {return a*b; });
  _globals[FNV1A("/")] = (Binary)([](const Var& a,const Var& b)->Var {return a/b; });
  _globals[FNV1A("%")] = (Binary)([](const Var& a,const Var& b)->Var {return a%b; });
  _globals[FNV1A("print")] = (Function)([](SymbolVar* stack,size_t params)->Var {
    for(uintptr_t i(0); i<params; i++)
      out << *stack[i];
    return 0;
  });
  _globals[FNV1A("typename")] = (Function)([](SymbolVar* stack,size_t params)->Var {
    return stack[0]->type()->name;
  });
  _globals[FNV1A("object")] = (Function)([](SymbolVar* stack,size_t params)->Var {
    Ref<Map<Var,Var> > wtr;
    wtr.make();
    Map<Var,Var>& map(*wtr);
    for(uintptr_t i(1); i<params; i += 2)
      map[*stack[i-1]] = *stack[i];
    return wtr;
  });
  _globals[FNV1A("now")] = (Native)([](Context& c,const Array<Var>&)->Var {
    return Time::now();
  });
  _globals[FNV1A("rand")] = (Native)([](Context& c,const Array<Var>&)->Var {
    return Rand::nextFloat();
  });
  _globals[FNV1A("mouse-x")] = (Native)([](Context& c,const Array<Var>&)->Var {
    return Window::mousePosition().x();
  });
  _globals[FNV1A("mouse-y")] = (Native)([](Context& c,const Array<Var>&)->Var {
    return Window::mousePosition().y();
  });
  _globals[FNV1A("mouse-set")] = (Function)([](SymbolVar* stack,size_t params)->Var {
    Window::mousePosition(Vector2i(stack[0]->get<int>(),stack[1]->get<int>()));
    return 0;
  });
  _globals[FNV1A("key-pressed")] = (Native)([](Context& c,const Array<Var>& a)->Var {
    if(a.size()==2 && a[1].is<Symbol>())
      switch(a[1].as<Symbol>()) {
#define KEY_BIND(k) case FNV1A(#k): return Window::isPressed(Window::Event::k)
        KEY_BIND(Z);
        KEY_BIND(Q);
        KEY_BIND(S);
        KEY_BIND(D);
        KEY_BIND(UP);
        KEY_BIND(DOWN);
        KEY_BIND(LEFT);
        KEY_BIND(RIGHT);
        KEY_BIND(SPACE);
      }
    return false;
  });
  _globals[FNV1A("vec")] = (Function)([](SymbolVar* stack,size_t params)->Var {
    if(params==3)
      return Vector3f(stack[0]->get<float>(),stack[1]->get<float>(),stack[2]->get<float>());
    return 0;
  });
}
