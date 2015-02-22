#ifndef DEF_L_Burp_VirtualMachine
#define DEF_L_Burp_VirtualMachine

#include <iostream>

#include "Function.h"
#include "../containers/Ref.h"
#include "../dynamic.h"
#include "../system/Directory.h"

namespace L{
    namespace Burp{
        class VirtualMachine{
            public:
                static Map<String,Ref<Function> > functions;
            private:
                static Directory working;
                static bool exit;

            public:
                static void read(std::istream& stream);
                static bool safe();
                static void init();
        };
    }
}

#endif






