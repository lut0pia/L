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
    class Symbol {
      private:
        const char* _str;
      public:
        inline Symbol() : _str(0) {}
        inline Symbol(const char* str) : _str(str) {}
        inline bool operator==(const Symbol& other) const {return _str==other._str;}
        inline const char* str() const {return _str;}
    };
    class Context {
      private:
        Array<KeyValue<Symbol,Var> > _stack;
        Array<int> _frames;
        Array<Symbol> _symbols;

      public:
        Context();
        void read(Stream&);
        void read(Var& v, Lexer& lexer);

        Symbol symbol(const char*);
        Var& variable(Symbol);
        void pushVariable(Symbol, const Var& = Var());
        inline void pushParameter(const Var& v) {pushVariable(symbol(""),v);}
        inline Var& Context::parameter(int i) {return _stack[currentFrame()+i].value();}
        inline int currentFrame() const {return _frames[_frames.size()-2];}
        inline int nextFrame() const {return _frames.back();}
        Var execute(const Var& code);
    };
  }
  inline Stream& operator<<(Stream& s, const Script::Symbol& v) {return s << v.str();}
  inline Stream& operator<<(Stream& s, const Script::Quote& v) {return s << '\'' << v.var;}
}

#endif

