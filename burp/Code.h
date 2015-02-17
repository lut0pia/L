#ifndef DEF_L_Burp_Code
#define DEF_L_Burp_Code

#include "../stl.h"
#include "../math.h"
#include "../containers/Ref.h"
#include "../dynamic.h"

namespace L{
    namespace Burp{
        using Dynamic::Variable;
        class Code{
            public:
                virtual Variable execute(Vector<Variable>&) = 0;

                static bool isConstExp(const String&);
                static Variable fromConstExp(const String&);
                virtual ~Code(){}

            protected:
                static size_t gIndex(const String& name, Map<String,size_t>&, size_t&);
                static Ref<Code> from(String code, Map<String,size_t>&, size_t&); // Returns code by reading a String
                static Vector<Variable> multiExecute(Vector<Ref<Code> >&,Vector<Variable>& locals);

            friend class VirtualMachine;
        };
    }
}

#endif






