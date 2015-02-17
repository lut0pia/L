#ifndef DEF_L_Burp_Function_Pointer
#define DEF_L_Burp_Function_Pointer

#include "Function.h"

namespace L{
    namespace Burp{
        class Function_Pointer : public Function{
            private:
                Variable (*f)(Vector<Variable>&);

            public:
                Function_Pointer(Variable (*)(Vector<Variable>&));
                Variable call(Vector<Variable>&);
        };
    }
}

#endif


