#ifndef DEF_L_Script_Context
#define DEF_L_Script_Context

#include "../containers/StaticStack.h"
#include "../containers/Ref.h"
#include "../dynamic/Variable.h"
#include "../containers/Map.h"
#include "../system/File.h"
#include "code/Base.h"
#include "Parser.h"

namespace L {
  namespace Script {
    class Context {
      private:
        Parser _parser;
        StaticStack<4096,Var> _stack;
        Map<String,Ref<Ref<Code::Base> > > _functions;

      public:
        Context();
        void read(const File&);
        void declare(const String&, Ref<Code::Base>);
        Ref<Code::Base>* function(const String&);
    };
  }
}

#endif

