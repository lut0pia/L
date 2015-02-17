#ifndef DEF_L_Burp_Function_Code
#define DEF_L_Burp_Function_Code

#include "Code.h"
#include "Function.h"

namespace L{
    namespace Burp{
        class Function_Code : public Function{
            private:
                Ref<Code> code;
                size_t scopeSize;
            public:
                Function_Code(Ref<Code> code, size_t scopeSize);
                virtual Dynamic::Variable call(Vector<Dynamic::Variable>& parameters);
        };
    }
}

#endif



