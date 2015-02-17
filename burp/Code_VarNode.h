#ifndef DEF_L_Burp_Code_VarNode
#define DEF_L_Burp_Code_VarNode

#include "Code.h"

namespace L{
    namespace Burp{
        class Code_VarNode : public Code{
            private:
                Vector<Ref<Code> > codes;

            public:
                Code_VarNode(String code, Map<String,size_t>&, size_t&);
                Variable execute(Vector<Variable>&);
        };
    }
}

#endif






