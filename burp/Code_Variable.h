#ifndef DEF_L_Burp_Code_Variable
#define DEF_L_Burp_Code_Variable

#include "Code.h"

namespace L{
    namespace Burp{
        class Code_Variable : public Code{
            private:
                size_t id;

            public:
                Code_Variable(String code, Map<String,size_t>& BurpVariableIndex, size_t& nextIndex);
                Variable execute(Array<Variable>&);
        };
    }
}

#endif






