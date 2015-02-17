#ifndef DEF_L_Burp_Code_If
#define DEF_L_Burp_Code_If

#include "Code.h"

namespace L{
    namespace Burp{
        class Code_If : public Code{
            private:
                Ref<Code> codes[3];
                bool hasElse;
            public:
                Code_If(String code, Map<String,size_t>& vi, size_t& ni);
                Variable execute(Vector<Variable>&);
        };
    }
}

#endif






