#pragma once

#include "Lexer.h"
#include "../containers/StaticStack.h"
#include "../dynamic/Variable.h"
#include "../hash.h"

namespace L {
  namespace Script {
    class Context;
    typedef uint32_t Symbol;
    typedef KeyValue<Symbol,Var> SymbolVar;
    typedef Var(*Function)(SymbolVar*,size_t);
    typedef Var(*Native)(Context&,const Array<Var>&);
    typedef Var(*Binary)(const Var&,const Var&);
    typedef struct { Var var; } Quote;
    typedef struct { Array<Symbol> parameters; Var code; } CodeFunction;
    class Context {
    private:
      static Map<Symbol,Var> _globals;
      StaticStack<128,SymbolVar> _stack;

    public:
      Context();
      void read(Stream&);
      void read(Var& v,Lexer& lexer);

      Var& variable(Symbol);
      inline Var& variable(const char* str){ return variable(fnv1a(str)); }
      Var& local(Symbol);
      Var execute(const Var& code);
      Var* reference(const Var& code);

      static inline Var& global(Symbol s){ return _globals[s]; }
    };
  }
  inline Stream& operator<<(Stream& s,const Script::Quote& v) { return s << '\'' << v.var; }
}


