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
    typedef Var(*Function)(Context&);
    typedef Var(*Native)(Context&, const Array<Var>&);
    typedef struct { uint32_t i; } Local;
    typedef struct { Symbol sym; } RawSymbol;
    typedef struct { Var code; uint32_t localCount; } CodeFunction;
    class Context {
    private:
      static Table<Symbol, Var> _globals;
      static Table<const TypeDescription*, Var> _typeTables;
      StaticStack<128, Var> _stack;
      StaticStack<16, Var> _selves;
      StaticStack<16, uint32_t> _frames;
      Var _self;

    public:
      Context();
      static CodeFunction read(Stream&);
      static void read(Var& v, Lexer& lexer);

      inline Var& currentSelf() { return (_selves.empty()) ? _self : _selves.top(); }
      inline Table<Var, Var>& selfTable() { return *_self.as<Ref<Table<Var, Var>>>(); }
      inline uint32_t currentFrame() const { return _frames.empty() ? 0 : _frames.top(); }
      inline uint32_t localCount() const { return _stack.size()-currentFrame(); }
      inline Var& local(uint32_t i) { return _stack.bottom(i+currentFrame()); }
      Var execute(const Var& code, Var* selfOut = nullptr);
      Var* reference(const Var& code, Var* selfOut = nullptr);
      bool tryExecuteMethod(const Symbol&, std::initializer_list<Var> = {});

      static inline Var& global(Symbol s) { return _globals[s]; }
      static Ref<Table<Var, Var>> typeTable(const TypeDescription*);
      static inline Var& typeValue(const TypeDescription* td, const Var& k) { return (*typeTable(td))[k]; }
    };
  }
  inline Stream& operator<<(Stream& s, const Script::RawSymbol& v) { return s << '\'' << v.sym; }
}


