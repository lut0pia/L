#ifndef DEF_L_Burp_Code_While
#define DEF_L_Burp_Code_While

#include "Code_Super.h"

namespace L{
    namespace Burp{
        class Code_While : public Code_Super<5,1>{
            public:
                Code_While(String code, Map<String,size_t>& vi, size_t& ni) : Code_Super(code,vi,ni){};
                Dynamic::Variable execute(Array<Dynamic::Variable>&);
        };
    }
}

#endif






