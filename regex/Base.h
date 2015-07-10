#ifndef DEF_L_Regex_Base
#define DEF_L_Regex_Base

#include "../containers/Ref.h"
#include "../containers/Set.h"
#include "../stl/String.h"

namespace L {
  namespace Regex {
    class Base {
      private:
        size_t _qMin, _qMax;
      public:
        Base(size_t qMin, size_t qMax);
        virtual ~Base() {}
        virtual Set<size_t> accepts(const String&, size_t) = 0; // Acceptance
        Set<size_t> qAccepts(const String&, const Set<size_t>&, size_t = 0); // Acceptance considering quantifier
        bool matches(const String&);
    };
  }
}

#endif

