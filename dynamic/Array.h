#ifndef DEF_L_Dynamic_Array
#define DEF_L_Dynamic_Array

#include "../containers/Array.h"
#include "Variable.h"

namespace L {
  namespace Dynamic {
    class Array : public L::Array<Variable> {
      public:
        Array& operator()(const Variable&);
        friend Stream& operator<<(Stream&, const Array&);
    };
    Stream& operator<<(Stream& s, const Array& v);
  }
}

#endif

