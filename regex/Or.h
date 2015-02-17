#ifndef DEF_L_Regex_Or
#define DEF_L_Regex_Or

#include "Base.h"

namespace L{
    namespace Regex{
        class Or : public Base{
            protected:
                Vector<Ref<Base> > expressions;
            public:
                Or(size_t qMin, size_t qMax, const Vector<Ref<Base> >& expressions);
                virtual Set<size_t> accepts(const String&, size_t);
        };
    }
}

#endif

