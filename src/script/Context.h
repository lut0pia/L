#pragma once

#include "Lexer.h"
#include "../containers/Array.h"
#include "../containers/KeyValue.h"
#include "../containers/StaticStack.h"
#include "../dynamic/Variable.h"
#include "../containers/Ref.h"
#include "../containers/Table.h"
#include "../text/Symbol.h"

namespace L {
  namespace Script {
    class Context;
    typedef KeyValue<Symbol,Var> SymbolVar;
    typedef Var(*Function)(const Var& src,SymbolVar* stack,size_t n);
    typedef Var(*Native)(Context&,const Array<Var>&);
    typedef struct { Var var; } Quote;
    typedef struct { Array<Symbol> parameters; Var code; } CodeFunction;
    class Context {
    private:
      static Table<Symbol,Var> _globals;
      static Table<const TypeDescription*,Var> _typeTables;
      StaticStack<128,SymbolVar> _stack;

    public:
      Context();
      void read(Stream&);
      void read(Var& v,Lexer& lexer);

      Var& variable(Symbol);
      inline Var& variable(const char* str){ return variable(Symbol(str)); }
      Var& local(Symbol);
      Var execute(const Var& code,Var* src = nullptr);
      Var* reference(const Var& code,Var* src = nullptr);

      static inline Var& global(Symbol s){ return _globals[s]; }
      static Ref<Table<Var,Var>> typeTable(const TypeDescription*);
      static inline Var& typeValue(const TypeDescription* td,const Var& k){ return (*typeTable(td))[k]; }
    };
  }
  inline Stream& operator<<(Stream& s,const Script::Quote& v) { return s << '\'' << v.var; }
}


