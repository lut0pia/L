#ifndef DEF_L_Script_Context
#define DEF_L_Script_Context

#include "../containers/StaticStack.h"
#include "Lexer.h"
#include "../containers/Map.h"
#include "../containers/Ref.h"
#include "../dynamic/Variable.h"

namespace L {
  namespace Script {
    typedef Var(*Native)(Var*);
    class Context {
      private:
        StaticStack<4096,Var> _stack;
        Map<String,Ref<Var> > _functions;

      public:
        void read(Stream&);
        void read(Var& v, Lexer& lexer);
        void declare(const String&, const Var&);
        Var* function(const String&);
        Var execute(const Var& code, Var* stack);
    };
  }
}

#endif

