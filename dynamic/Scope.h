#ifndef DEF_L_Dynamic_Scope
#define DEF_L_Dynamic_Scope

#include "../stl.h"
#include "Variable.h"

namespace L{
    namespace Dynamic{
        class Scope{
            private:
                static List<Map<String,Var> > stack;
                Map<String,Var>& scope;
            public:
                Scope();
                Var& operator[](const String&);
                ~Scope();
        };
    }
}

#endif

