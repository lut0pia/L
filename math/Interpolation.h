#ifndef DEF_L_Interpolation
#define DEF_L_Interpolation

#include "../geometry/Vector.h"
#include "../tmp.h"

namespace L {
  template <int d, class T>
  class Interpolation {
      static const int d2 = static_pow<2,d>::value;
    public:
      static T linear(T values[d2], Vector<d,float> weight) {
        int wd(d-1);
        for(int i(d2/2); i>0; i>>=1) {
          for(int j(0); j<i; j++)
            values[j] = values[j]*(1.f - weight[wd]) + values[j+i]*weight[wd];
          wd--;
        }
        return values[0];
      }
  };
}

#endif





