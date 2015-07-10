#ifndef DEF_L_Regex_Char
#define DEF_L_Regex_Char

#include "Base.h"

namespace L {
  namespace Regex {
    class Char : public Base {
      protected:
        Set<char> chars;
      public:
        Char(size_t qMin, size_t qMax); // Any character
        Char(size_t qMin, size_t qMax, char c); // One character
        Char(size_t qMin, size_t qMax, const Set<char>& chars); // Set of character
        virtual Set<size_t> accepts(const String&, size_t);
    };
  }
}

#endif

