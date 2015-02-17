#ifndef DEF_L_Burp_Code_VarRef
#define DEF_L_Burp_Code_VarRef

#include "Code.h"

namespace L{
    namespace Burp{
        class Code_VarRef : public Code{
            private:
                size_t id;

            public:
                Code_VarRef(String code, Map<String,size_t>&, size_t&);
                Variable execute(Vector<Variable>&);
        };
    }
}

#endif






