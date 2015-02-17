#ifndef DEF_L_Burp_Code_ConstValue
#define DEF_L_Burp_Code_ConstValue

#include "Code.h"

namespace L{
    namespace Burp{
        class Code_ConstValue : public Code{
            private:
                Variable value;

            public:
                Code_ConstValue(String code, Map<String,size_t>&, size_t&);
                virtual Variable execute(Vector<Variable>&);
        };
    }
}

#endif






