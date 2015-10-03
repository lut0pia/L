#ifndef DEF_L_Script_Code_Base
#define DEF_L_Script_Code_Base

#include "../../containers/Ref.h"
#include "../../dynamic/Variable.h"

namespace L {
  namespace Script {
    class Context;
    class Compiler;
    namespace Code {
      class Base {
        public:
          virtual Var execute(Context&) = 0;
          virtual ~Base() {}

          static Ref<Base> from(Compiler&);
      };
    }
  }
}

#endif

