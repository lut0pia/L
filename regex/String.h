#ifndef DEF_L_Regex_String
#define DEF_L_Regex_String

#include "Base.h"
#include "../containers/Array.h"

namespace L {
  namespace Regex {
    class String : public Base {
      protected:
        Array<Ref<Base> > expressions;
      public:
        String(size_t qMin, size_t qMax, const Array<Ref<Base> >& expressions);
        virtual Set<size_t> accepts(const L::String&, size_t);
    };
  }
}

#endif

