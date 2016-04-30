#include "Context.h"

#include "../streams/FileStream.h"

using namespace L;
using namespace Script;

Context::Context() : _frames(2,0) {
  // Local allows to define local variables without overriding more global variables
  variable(FNV1A("local")) = (Native)([](Context& c,const Array<Var>& a)->Var {
    Var value;
    if(a.size()>1 && a[1].is<Symbol>()){
      if(a.size()>2)
        value = c.execute(a[2]);
      c.pushVariable(a[1].as<Symbol>(),value); // Add variable to frame
    }
    return value;
  });
  variable(FNV1A("do")) = (Native)([](Context& c,const Array<Var>& a)->Var {
    for(uint32_t i(1); i<a.size()-1; i++)
      c.execute(a[i]);
    return c.execute(a.back());
  });
  variable(FNV1A("while")) = (Native)([](Context& c,const Array<Var>& a)->Var {
    Var wtr;
    while(c.execute(a[1]).get<bool>())
      wtr = c.execute(a[2]);
    return wtr;
  });
  variable(FNV1A("if")) = (Native)([](Context& c,const Array<Var>& a)->Var {
    if(a.size()>2) {
      if(c.execute(a[1]).get<bool>())
        return c.execute(a[2]);
      else if(a.size()>3)
        return c.execute(a[3]);
    }
    return 0;
  });
  variable(FNV1A("set")) = (Native)([](Context& c,const Array<Var>& a)->Var {
    Var* target;
    if(a.size()==3 && (target = c.reference(a[1])))
      return *target = c.execute(a[2]);
    return 0;
  });
  variable(FNV1A("fun")) = (Native)([](Context& c,const Array<Var>& a)->Var {
    if(a.size()>1){
      CodeFunction wtr;
      if(a.size()>2 && a[1].is<Array<Var> >()) // There's a list of parameter symbols
        for(auto&& sym : a[1].as<Array<Var> >())
          if(sym.is<Symbol>())
            wtr.parameters.push(sym.as<Symbol>());
      wtr.code = a.back(); // The last part is always the code
      return wtr;
    }
    return 0;
  });
  variable(FNV1A("=")) = (Binary)([](const Var& a,const Var& b)->Var {return a==b; });
  variable(FNV1A("<>")) = (Binary)([](const Var& a,const Var& b)->Var {return a!=b; });
  variable(FNV1A(">")) = (Binary)([](const Var& a,const Var& b)->Var {return a>b; });
  variable(FNV1A("<")) = (Binary)([](const Var& a,const Var& b)->Var {return a<b; });
  variable(FNV1A(">=")) = (Binary)([](const Var& a,const Var& b)->Var {return a>=b; });
  variable(FNV1A("<=")) = (Binary)([](const Var& a,const Var& b)->Var {return a<=b; });
  variable(FNV1A("+")) = (Binary)([](const Var& a,const Var& b)->Var {return a+b; });
  variable(FNV1A("-")) = (Binary)([](const Var& a,const Var& b)->Var {return a-b; });
  variable(FNV1A("*")) = (Binary)([](const Var& a,const Var& b)->Var {return a*b; });
  variable(FNV1A("/")) = (Binary)([](const Var& a,const Var& b)->Var {return a/b; });
  variable(FNV1A("%")) = (Binary)([](const Var& a,const Var& b)->Var {return a%b; });
  variable(FNV1A("print")) = (Function)([](Context& c,int params)->Var {
    for(int i(0); i<params; i++)
      out << c.parameter(i);
    return 0;
  });
  variable(FNV1A("typename")) = (Function)([](Context& c,int params)->Var {
    return c.parameter(0).type()->name;
  });
  variable(FNV1A("object")) = (Function)([](Context& c,int params)->Var {
    Var wtr;
    wtr.make<Map<Var,Var> >();
    Map<Var,Var>& map(wtr.as<Map<Var,Var> >());
    for(int i(1); i<params; i += 2)
      map[c.parameter(i-1)] = c.parameter(i);
    return wtr;
  });
}
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
    v = Array<Var>();
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
    else if(lexer.isToken("true")) v = true;
    else if(lexer.isToken("false")) v = false;
    else if(token[strspn(token,"-0123456789")]=='\0') v = atoi(token); // Integer
    else if(token[strspn(token,"-0123456789.")]=='\0') v = (float)atof(token); // Float
    else v = fnv1a(token);
    lexer.nextToken();
  }
}

Var& Context::variable(Symbol sym) {
  // Search the known variable
  for(int i(nextFrame()-1); i>=0; i--)
    if(_stack[i].key()==sym)
      return _stack[i].value();
  pushVariable(sym);
  return _stack.back().value();
}
void Context::pushVariable(Symbol sym,const Var& v) {
  _stack.push(keyValue(sym,v));
  _frames.back()++; // Added variable to current frame, must push next frame
}
Var Context::execute(const Var& code) {
  if(Var* ref = reference(code)) return *ref; // 
  else if(code.is<Array<Var> >()) { // Function call
    const Array<Var>& array(code.as<Array<Var> >()); // Get reference of array value
    const Var& handle(execute(array[0])); // Execute first child of array to get function handle
    if(handle.is<Native>())
      return handle.as<Native>()(*this,array);
    else if(handle.is<Function>() || handle.is<CodeFunction>()) {
      Var wtr;
      _frames.push(nextFrame()); // Add new frame
      for(uint32_t i(1); i<array.size(); i++) { // For all parameters
        Symbol sym = {0};
        if(handle.is<CodeFunction>() && handle.as<CodeFunction>().parameters.size()>=i)
          sym = handle.as<CodeFunction>().parameters[i-1];
        pushVariable(sym,execute(array[i])); // Compute parameter values
      }
      //out << _stack << '\n';
      if(handle.is<CodeFunction>())
        wtr = execute(handle.as<CodeFunction>().code);
      else if(handle.is<Function>()) // It's a function pointer
        wtr = handle.as<Function>()(*this,array.size()-1); // Call function
      _frames.pop(); // Remove current frame
      _stack.size(nextFrame()); // Remove elements from previous frame
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
    const Array<Var> array(code.as<Array<Var> >());
    if(array.size()==2){ // It's a pair
      if(Var* first = reference(array[0])){ // The first is a reference
        if(first->is<Map<Var,Var> >()) // It's an object
          return &first->as<Map<Var,Var> >()[execute(array[1])]; // Compute index and return pointer to field
        else if(first->is<Array<Var> >()) // It's an array
          return &first->as<Array<Var> >()[execute(array[1]).get<int>()]; // Compute index and return pointer to element
      }
    }
  }
  return nullptr;
}