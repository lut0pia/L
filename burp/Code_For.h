#ifndef DEF_L_Burp_Code_For
#define DEF_L_Burp_Code_For

#include "Code_Super.h"

namespace L{
    namespace Burp{
        class Code_For : public Code_Super<3,3>{
            public:
                Code_For(String code, Map<String,size_t>& vi, size_t& ni) : Code_Super(code,vi,ni){};
                Variable execute(Vector<Variable>&);
        };
    }
}

#endif






