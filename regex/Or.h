#ifndef DEF_L_Regex_Or
#define DEF_L_Regex_Or

#include "Base.h"
#include "../containers/Array.h"

namespace L{
    namespace Regex{
        class Or : public Base{
            protected:
                Array<Ref<Base> > expressions;
            public:
                Or(size_t qMin, size_t qMax, const Array<Ref<Base> >& expressions);
                virtual Set<size_t> accepts(const String&, size_t);
        };
    }
}

#endif

