#ifndef DEF_L_Dynamic_Array
#define DEF_L_Dynamic_Array

#include "../containers/Array.h"
#include "Variable.h"

namespace L {
  namespace Dynamic {
    class Array : public L::Array<Variable> {
      public:
        Array& operator()(const Variable&);
        friend std::ostream& operator<<(std::ostream&, const Array&);
    };
    std::ostream& operator<<(std::ostream& s, const Array& v);
    L_TYPE_OUTABLE(Array)
  }
}

#endif

