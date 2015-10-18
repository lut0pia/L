#ifndef DEF_L_Volume
#define DEF_L_Volume

#include "../Exception.h"
#include "Vector.h"

namespace L {
  template <int di,class T>
  class Volume {
    protected:
      Vector<di,T> a, b, c, d;
    public:
      Volume() {
      }
      Volume(const Vector<di,T>& a,
             const Vector<di,T>& b,
             const Vector<di,T>& c,
             const Vector<di,T>& d)
        : a(a), b(b), c(c), d(d) {
      }

      bool contains(const Vector<di,T>& p) {
        throw Exception("This feature hasn't been done yet.");
      }
      bool empty() const {
        return ((a == b) && (b == c) && (c == d));
      }

      inline const Vector<di,T>& gA() const {return a;}
      inline const Vector<di,T>& gB() const {return b;}
      inline const Vector<di,T>& gC() const {return c;}
      inline const Vector<di,T>& gD() const {return d;}
  };
}

#endif
