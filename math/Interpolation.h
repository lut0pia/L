#ifndef DEF_L_Interpolation
#define DEF_L_Interpolation

#include "../geometry/Point.h"
#include "../tmp.h"

namespace L {
  template <int d, class T>
  class Interpolation {
      static const int d2 = static_pow<2,d>::value;
    public:
      static T lerp(T a0, T a1, float w) {
        return (1.f - w)*a0 + w*a1;
      }
      static T compute(T values[d2], Point<d,float> weight) {
        int wd(d-1);
        for(int i(d2/2); i>0; i/=2) {
          for(int j(0); j<i; j++)
            values[j] = lerp(values[j],values[j+i],weight[wd]);
          wd--;
        }
        return values[0];
      }
  };
}

#endif





