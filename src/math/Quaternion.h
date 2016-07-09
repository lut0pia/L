#pragma once

#include "math.h"
#include "Vector.h"

namespace L {
  template <class T>
  class Quaternion : public Vector<4,T> {
    private:
      inline Quaternion(const T& x, const T& y, const T& z, const T& w) : Vector<4,T>(x,y,z,w) {}
    public:
      inline Quaternion() : Vector<4,T>(0,0,0,1) {}
      inline Quaternion(const Vector<3,T>& v) : Vector<4,T>(v.x(),v.y(),v.z(),0) {}
      Quaternion(const Vector<3,T>& axis, const T& angle) {
        this->w() = cos(angle/2);
        this->x() = axis.x()*sin(angle/2);
        this->y() = axis.y()*sin(angle/2);
        this->z() = axis.z()*sin(angle/2);
      }
      Quaternion operator*(const Quaternion& other) const {
        return Quaternion(this->w()*other.x()+this->x()*other.w()+this->y()*other.z()-this->z()*other.y(),
                          this->w()*other.y()-this->x()*other.z()+this->y()*other.w()+this->z()*other.x(),
                          this->w()*other.z()+this->x()*other.y()-this->y()*other.x()+this->z()*other.w(),
                          this->w()*other.w()-this->x()*other.x()-this->y()*other.y()-this->z()*other.z());
      }

      inline Quaternion inverse() const {return Quaternion(-this->x(),-this->y(),-this->z(),this->w());}
      inline Vector<3,T> rotate(const Vector<3,T>& v) const {return (*this)*v*inverse();}
      operator Vector<3,T>() const {return Vector<3,T>(this->x(),this->y(),this->z());}
  };
  typedef Quaternion<float> Quatf;
}
