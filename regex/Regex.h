#ifndef DEF_L_Regex
#define DEF_L_Regex

#include "Base.h"

namespace L{
    namespace Regex{
        Ref<Base> from(const L::String&, size_t qMin=1, size_t qMax=1);
    }
}

#endif

