#ifndef DEF_L_Script_Context
#define DEF_L_Script_Context

#include "Lexer.h"
#include "../dynamic/Variable.h"

namespace L {
  namespace Script {
    class Context;
    typedef Var(*Function)(Context&,int);
    typedef Var(*Native)(Context&,const Array<Var>&);
    typedef struct {Var var;} Quote;
    typedef struct {uint32 id;} Symbol;
    class Context {
      private:
        Array<KeyValue<Symbol,Var> > _stack;
        Array<int> _frames;

      public:
        Context();
        void read(Stream&);
        void read(Var& v, Lexer& lexer);

        static Symbol symbol(const char*);
        Var& variable(Symbol);
        void pushVariable(Symbol, const Var& = Var());
        inline void pushParameter(const Var& v) {pushVariable(symbol(""),v);}
        inline Var& parameter(int i) {return _stack[currentFrame()+i].value();}
        inline int currentFrame() const {return _frames[_frames.size()-2];}
        inline int nextFrame() const {return _frames.back();}
        Var execute(const Var& code);
    };
  }
  inline Stream& operator<<(Stream& s, const Script::Symbol& v) {return s << v.id;}
  inline Stream& operator<<(Stream& s, const Script::Quote& v) {return s << '\'' << v.var;}
}

#endif

