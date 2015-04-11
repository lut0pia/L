#ifndef DEF_L_Surface
#define DEF_L_Surface

#include "../Exception.h"
#include "Point.h"

namespace L {
  template <int d,class T>
  class Surface {
    protected:
      Point<d,T> a, b, c;
    public:
      Surface() {
        if(d<2) throw sex;
      }
      Surface(const Point<d,T>& a,
              const Point<d,T>& b,
              const Point<d,T>& c)
        : a(a), b(b), c(c) {
      }

      bool contains(const Point<d,T>& p) {
        Point<d,T> ap(p-a), ab(b-a), ac(c-a);
        Point<2,T> sap; // sap is a vector defining ap according to abc
        if(d==2) {
          sap.x() = -(ac.y()*ap.x() - ac.x()*ap.y())/(ab.y()*ac.x() - ab.x()*ac.y());
          sap.y() = (ab.y()*ap.x() - ab.x()*ap.y())/(ab.y()*ac.x() - ab.x()*ac.y());
        } else {
          // Solve: sap.x*ab.v[i] + sap.y*ac.v[i] = ap.v[i]
          // by eliminating all equivalent equations
          throw Exception("This feature hasn't been done yet.");
        }
        return (sap.x()+sap.y()<=1.0 && sap.x()>=0.0 && sap.y()>=0.0);
      }
      bool empty() const {
        return ((a == b) && (b == c));
      }

      inline const Point<d,T>& gA() const {return a;}
      inline const Point<d,T>& gB() const {return b;}
      inline const Point<d,T>& gC() const {return c;}
  };
}

#endif
