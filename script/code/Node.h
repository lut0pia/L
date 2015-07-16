#ifndef DEF_L_Script_Code_Node
#define DEF_L_Script_Code_Node

#include "Base.h"
#include "../../containers/Array.h"

namespace L {
  namespace Script {
    namespace Code {
      class Node : public Base {
        private:
          Array<Ref<Base> > _codes;
        public:
          Var execute(Context&);
          virtual ~Node() {}
      };
    }
  }
}

#endif

