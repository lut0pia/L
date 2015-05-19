#ifndef DEF_L_Burp_Function
#define DEF_L_Burp_Function

#include "../dynamic.h"

namespace L{
    namespace Burp{
        using Dynamic::Variable;
        class Function{
            public:
                virtual ~Function(){}
                virtual Variable call(Array<Variable>& parameters) = 0;
        };
    }
}

#endif






