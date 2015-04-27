#ifndef DEF_L_Perlin
#define DEF_L_Perlin

#include "../geometry/Point.h"
#include "../containers/Array.h"
#include "../tmp.h"

namespace L {
  template <int d>
  class Perlin {
    private:
      static const int d2 = static_pow<2,d>::value;
      Array<d,Point<d,float> > _gradients;
      float _size;
    public:
      Perlin(int size) : _size(size) {
        _gradients.resize(Point<d,int>(size));
        for(int i(0); i<_gradients.size(); i++) {
          for(int j(0); j<d; j++)
            _gradients[i][j] = Rand::next(-1.f,1.f);
          _gradients[i].normalize();
        }
      }
      float lerp(float a0, float a1, float w) const {
        return (1.f - w)*a0 + w*a1;
      }
      float value(Point<d,float> point) const {
        // Determine grid cell coordinates
        Point<d,int> i0,i1;
        for(int i(0); i<d; i++) {
          point[i] = PMod(point[i],_size); // Make sure the point falls into available values
          i0[i] = (int)point[i];
          if(point[i]<0)
            i0[i] -= 1;
          i1[i] = i0[i]+1;
        }
        // Compute values for near neighbors
        Point<d,float> s(point-Point<d,float>(i0));
        float values[d2];
        for(int i(0); i<d2; i++) {
          Point<d,int> ip;
          for(int j(0); j<d; j++)
            ip[j] = ((i&(1<<j))?i1[j]:i0[j]);
          values[i] = _gradients(ip).dot(point-Point<d,float>(ip));
        }
        // Interpolate all dimensions
        int wd(d-1);
        for(int i(d2/2); i>0; i/=2) {
          for(int j(0); j<i; j++)
            values[j] = lerp(values[j],values[j+i],s[wd]);
          wd--;
        }
        return values[0];
      }
  };
}

#endif





