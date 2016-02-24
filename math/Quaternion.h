#ifndef DEF_L_Quaternion
#define DEF_L_Quaternion

#include "Vector.h"

namespace L {
  template <class T>
  class Quaternion : private Vector<4,T> {
    private:
      inline Quaternion(const T& x, const T& y, const T& z, const T& w) : Vector<4,T>(x,y,z,w) {}
    public:
      inline Quaternion() : Vector<4,T>(0,0,0,1) {}
      inline Quaternion(const Vector<3,T>& v) : Vector<4,T>(v.x(),v.y(),v.z(),0) {}
      Quaternion(const Vector<3,T>& axis, const T& angle) {
        w() = cos(angle/2);
        x() = axis.x()*sin(angle/2);
        y() = axis.y()*sin(angle/2);
        z() = axis.z()*sin(angle/2);
      }
      Quaternion operator*(const Quaternion& other) const {
        return Quaternion(w()*other.x()+x()*other.w()+y()*other.z()-z()*other.y(),
                          w()*other.y()-x()*other.z()+y()*other.w()+z()*other.x(),
                          w()*other.z()+x()*other.y()-y()*other.x()+z()*other.w(),
                          w()*other.w()-x()*other.x()-y()*other.y()-z()*other.z());
      }

      inline Quaternion inverse() const {return Quaternion(-x(),-y(),-z(),w());}
      inline Vector<3,T> rotate(const Vector<3,T>& v) const {return (*this)*v*inverse();}
      operator Vector<3,T>() const {return Vector<3,T>(x(),y(),z());}
  };
  typedef Quaternion<float> Quatf;
}

#endif
