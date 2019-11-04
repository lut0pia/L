#pragma once

#include "math.h"
#include "Vector.h"

namespace L {
  template <class T>
  class Quaternion : public Vector<4,T> {
  public:
    inline Quaternion() : Vector<4, T>(T(0), T(0), T(0), T(1)) {}
    inline Quaternion(const T& x,const T& y,const T& z,const T& w) : Vector<4,T>(x,y,z,w) {}
    inline Quaternion(const Vector<3, T>& v) : Vector<4, T>(v.x(), v.y(), v.z(), 0) {}
    inline Quaternion(const Vector<4, T>& v) : Vector<4, T>(v) {}
    Quaternion(const Vector<3,T>& axis,const T& angle) {
      this->w() = cos(angle/2);
      const float sa2(sin(angle/2));
      this->x() = axis.x()*sa2;
      this->y() = axis.y()*sa2;
      this->z() = axis.z()*sa2;
    }
    Quaternion operator*(const Quaternion& other) const {
      return Quaternion(this->w()*other.x()+this->x()*other.w()+this->y()*other.z()-this->z()*other.y(),
                        this->w()*other.y()-this->x()*other.z()+this->y()*other.w()+this->z()*other.x(),
                        this->w()*other.z()+this->x()*other.y()-this->y()*other.x()+this->z()*other.w(),
                        this->w()*other.w()-this->x()*other.x()-this->y()*other.y()-this->z()*other.z());
    }

    inline Quaternion inverse() const { return Quaternion(-this->x(),-this->y(),-this->z(),this->w()); }
    inline Vector<3,T> rotate(const Vector<3,T>& v) const {
      Vector<3, T> t, u;
      t.x() = (this->y()*v.z() - this->z()*v.y())*T(2);
      t.y() = (this->z()*v.x() - this->x()*v.z())*T(2);
      t.z() = (this->x()*v.y() - this->y()*v.x())*T(2);
      u.x() = (this->y()*t.z() - this->z()*t.y());
      u.y() = (this->z()*t.x() - this->x()*t.z());
      u.z() = (this->x()*t.y() - this->y()*t.x());
      t.x() *= this->w(); t.y() *= this->w(); t.z() *= this->w();
      t.x() += v.x(); t.y() += v.y(); t.z() += v.z();
      t.x() += u.x(); t.y() += u.y(); t.z() += u.z();
      return t;
    }
    void to_axis_angle(Vector<3, T>& axis, T& angle) const {
      angle = 2 * acos(this->w());
      if(angle > 0.1) {
        const T t(1/sqrt(1-sqr(this->w())));
        axis.x() = this->x() * t;
        axis.y() = this->y() * t;
        axis.z() = this->z() * t;
      } else {
        axis.x() = 1;
        angle = axis.y() = axis.z() = 0;
      }
    }
    Vector<3, T> to_scaled_vector() const {
      Vector<3, T> v;
      float a;
      to_axis_angle(v, a);
      v.length(a);
      return v;
    }
    inline operator Vector<3,T>() const { return Vector<3,T>(this->x(),this->y(),this->z()); }
  };

  template <class T> Stream& operator<<(Stream& s, const Quaternion<T>& v) { return s << (const Vector<4, T>&)v; }
  template <class T> Stream& operator<(Stream& s, const Quaternion<T>& v) { return s < (const Vector<4, T>&)v; }
  template <class T> Stream& operator>(Stream& s, Quaternion<T>& v) { return s > (Vector<4, T>&)v; }

  typedef Quaternion<float> Quatf;
}
