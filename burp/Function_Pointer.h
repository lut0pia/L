#ifndef DEF_L_Burp_Function_Pointer
#define DEF_L_Burp_Function_Pointer

#include "Function.h"

namespace L{
    namespace Burp{
        class Function_Pointer : public Function{
            private:
                Variable (*f)(Array<Variable>&);

            public:
                Function_Pointer(Variable (*)(Array<Variable>&));
                Variable call(Array<Variable>&);
        };
    }
}

#endif


