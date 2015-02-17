#ifndef DEF_L_Regex_String
#define DEF_L_Regex_String

#include "Base.h"

namespace L{
    namespace Regex{
        class String : public Base{
            protected:
                Vector<Ref<Base> > expressions;
            public:
                String(size_t qMin, size_t qMax, const Vector<Ref<Base> >& expressions);
                virtual Set<size_t> accepts(const L::String&, size_t);
        };
    }
}

#endif

