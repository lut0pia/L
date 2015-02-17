#ifndef DEF_L_Burp_Code_FunctionCall
#define DEF_L_Burp_Code_FunctionCall

#include "Code.h"
#include "Function.h"

namespace L{
    namespace Burp{
        class Code_FunctionCall : public Code{
            private:
                Ref<Function>* function;
                Vector<Ref<Code> > parameters;

            public:
                Code_FunctionCall(String code, Map<String,size_t>&, size_t&);
                Variable execute(Vector<Variable>&);
        };
    }
}

#endif







