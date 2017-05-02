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
    typedef void(*Function)(Context&);
    typedef void(*Native)(Context&, const Array<Var>&);
    typedef struct { uint32_t i; } Local;
    typedef struct { Symbol sym; } RawSymbol;
    typedef struct { Var code; uint32_t localCount; } CodeFunction;
    class Context {
    private:
      static Table<Symbol, Var> _globals;
      static Table<const TypeDescription*, Var> _typeTables;
      static StaticStack<128, Var> _stack;
      static StaticStack<16, uint32_t> _frames;
      static StaticStack<16, Var> _selves;
      Var _self;

    public:
      Context();
      static CodeFunction read(Stream&);
      static void read(Var& v, Lexer& lexer);

      inline Var& currentSelf() { L_ASSERT(!_selves.empty()); return _selves.top(); }
      inline Table<Var, Var>& selfTable() { return *_self.as<Ref<Table<Var, Var>>>(); }
      inline uint32_t currentFrame() const { return _frames.empty() ? 0 : _frames.top(); }
      inline uint32_t localCount() const { return _stack.size()-currentFrame(); }
      inline Var& local(uint32_t i) { return _stack.bottom(i+currentFrame()); }
      inline Var& returnValue() { return local(uint32_t(-1)); }
      bool tryExecuteMethod(const Symbol&, std::initializer_list<Var> = {});
      Var executeInside(const Var& code);

      static inline Var& global(Symbol s) { return _globals[s]; }
      static Ref<Table<Var, Var>> typeTable(const TypeDescription*);
      static inline Var& typeValue(const TypeDescription* td, const Var& k) { return (*typeTable(td))[k]; }
    
    private:
      Var executeReturn(const Var& code); // Copies and return result of execution
      Var& executeRef(const Var& code); // Pushes result of exection on stack then returns ref to it
      void discardExecute(const Var& code); // Replaces top of the stack with result of execution
      void executeDiscard(const Var& code); // Discards result of execution
      void execute(const Var& code, Var* selfOut = nullptr); // Pushes result of execution on stack
      Var* reference(const Var& code, Var* selfOut = nullptr);
    };
  }
  inline Stream& operator<<(Stream& s, const Script::RawSymbol& v) { return s << '\'' << v.sym; }
}


