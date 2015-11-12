#ifndef DEF_L_Interpolation
#define DEF_L_Interpolation

#include "../geometry/Vector.h"
#include "../tmp.h"

namespace L {
  template <class T>
  class Interpolation {
    public:
      template <int d>
      static T linear(const T* v, const float* w) {
        T tmp[2];
        for(int i(0); i<2; i++)
          tmp[i] = Interpolation<T>::linear<d-1>(v+i*2,w+1);
        return Interpolation<T>::linear<1>(tmp,w);
      }
      template <> inline static T linear<1>(const T* v, const float* w) {
        return v[0]*(1.f-w[0]) + v[1]*w[0];
      }
      template <int d>
      static T cubic(const T* v, const float* w) {
        T tmp[4];
        for(int i(0); i<4; i++)
          tmp[i] = Interpolation<T>::cubic<d-1>(v+i*4,w+1);
        return Interpolation<T>::cubic<1>(tmp,w);
      }
      template <> inline static T cubic<1>(const T* v, const float* w) {
        return v[1]+.5f*w[0]*(v[2]-v[0]+w[0]*(2.f*v[0]-5.f*v[1]+4.f*v[2]-v[3]+w[0]*(3.f*(v[1]-v[2])+v[3]-v[0])));
      }
  };
}

#endif





