#ifndef DEF_L_Dynamic_Array
#define DEF_L_Dynamic_Array

#include <vector>
#include "Variable.h"

namespace L {
  namespace Dynamic {
    class Array : public Vector<Variable> {
      public:
        Array& operator()(const Variable&);
        friend std::ostream& operator<<(std::ostream&, const Array&);
    };
    std::ostream& operator<<(std::ostream& s, const Array& v);
    L_TYPE_OUTABLE(Array)
  }
}

#endif

