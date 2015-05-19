#ifndef DEF_L_Burp_Code_Node
#define DEF_L_Burp_Code_Node

#include "Code.h"

namespace L{
    namespace Burp{
        class Code_Node : public Code{
            private:
                Array<Ref<Code> > codes;

            public:
                Code_Node(String code, Map<String,size_t>&, size_t&);
                Variable execute(Array<Variable>&);
        };
    }
}

#endif






