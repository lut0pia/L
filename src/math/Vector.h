#pragma once

#include <cmath>
#include <limits>
#include "../types.h"
#include "../math/Rand.h"
#include "../streams/Stream.h"

namespace L {
  template <int d,class T>
  class Vector {
  protected:
    T _c[d];
  public:
    inline Vector() = default;
    template <class R>
    inline Vector(const Vector<d,R>& other) {
      for(int i(0); i<d; i++)
        _c[i] = T(other[i]);
    }
    inline Vector(const T& v) {
      for(int i(0); i<d; i++)
        _c[i] = v;
    }
    template <typename... Args>
    inline Vector(const T& v,Args&&... args) {
      _c[0] = v;
      new(((T*)this)+1)Vector<d-1,T>(args...);
    }
    inline Vector(const Vector<d-1,T>& p,const T& w = 1.0) {
      for(int i(0); i<d-1; i++)
        _c[i] = p[i];
      _c[d-1] = w;
    }
    inline Vector(const Vector<d+1,T>& p) {
      for(int i(0); i<d; i++)
        _c[i] = p[i];
    }

    Vector& operator+=(const Vector& other) {
      for(int i(0); i<d; i++)
        _c[i] += other._c[i];
      return *this;
    }
    Vector& operator-=(const Vector& other) {
      for(int i(0); i<d; i++)
        _c[i] -= other._c[i];
      return *this;
    }
    Vector& operator*=(const Vector& other) {
      for(int i(0); i<d; i++)
        _c[i] *= other._c[i];
      return *this;
    }
    Vector& operator/=(const Vector& other) {
      for(int i(0); i<d; i++)
        _c[i] /= other._c[i];
      return *this;
    }
    inline Vector operator+(const Vector& other) const { return Vector(*this) += other; }
    inline Vector operator-(const Vector& other) const { return Vector(*this) -= other; }
    Vector operator-() const {
      Vector wtr;
      for(int i(0); i<d; i++)
        wtr._c[i] = -_c[i];
      return wtr;
    }
    inline Vector operator*(const Vector& other) const { return Vector(*this) *= other; }
    inline Vector operator/(const Vector& other) const { return Vector(*this) /= other; }

    bool operator==(const Vector& other) const {
      for(int i(0); i<d; i++)
        if(_c[i] != other._c[i])
          return false;
      return true;
    }
    inline bool operator!=(const Vector& other) const { return !(*this == other); }
    bool operator<(const Vector& other) const {
      for(int i(0); i<d; i++)
        if(_c[i] < other._c[i])
          return true;
        else if(_c[i] > other._c[i])
          return false;
      return false;
    }
    inline bool operator>(const Vector& other) const { return other<*this; }
    T lengthSquared() const {
      T wtr(0);
      for(int i(0); i<d; i++)
        wtr += _c[i]*_c[i];
      return wtr;
    }
    inline T length() const { return std::sqrt(lengthSquared()); }
    Vector& length(const T& s){
      normalize();
      return this->operator*=(s);
    }
    T manhattan() const {
      T wtr(0);
      for(int i(0); i<d; i++)
        wtr += L::abs(_c[i]);
      return wtr;
    }
    Vector& normalize() {
      T n(length());
      for(int i(0); i<d; i++)
        _c[i] /= n;
      return *this;
    }
    inline Vector normalized() const { return Vector(*this).normalize(); }
    inline T dist(const Vector& other) const { return (*this-other).length(); }
    inline T distSquared(const Vector& other) const { return (*this-other).lengthSquared(); }
    inline Vector cross(const Vector& other) const {
      return Vector(y()*other.z() - z()*other.y(),
                    z()*other.x() - x()*other.z(),
                    x()*other.y() - y()*other.x());
    }
    T dot(const Vector& other) const {
      T wtr(0);
      for(int i(0); i<d; i++)
        wtr += _c[i]*other._c[i];
      return wtr;
    }
    T product() const {
      T wtr(1);
      for(int i(0); i<d; i++)
        wtr *= _c[i];
      return wtr;
    }
    T sum() const {
      T wtr(0);
      for(int i(0); i<d; i++)
        wtr += _c[i];
      return wtr;
    }
    inline Vector reflect(const Vector& v) const { return ((*this*(dot(v)*T(2)))-v); }
    Vector abs() const {
      Vector wtr;
      for(int i(0); i<d; i++)
        wtr[i] = L::abs(_c[i]);
      return wtr;
    }
    bool increment(const Vector& min,const Vector& max,const Vector& delta = 1) {
      for(int i(0); i<d; i++) {
        _c[i] += delta[i];
        if(_c[i]>=max._c[i]) {
          _c[i] = min._c[i];
          if(i==d-1)
            return false;
        } else break;
      }
      return true;
    }

    inline const T& operator[](int i) const { return _c[i]; }
    inline const T& x() const { return _c[0]; }
    inline const T& y() const { return _c[1]; }
    inline const T& z() const { return _c[2]; }
    inline const T& w() const { return _c[3]; }
    inline const byte* bytes() const { return (const byte*)&_c; }
    inline const T* array() const { return _c; }
    inline T& operator[](int i) { return _c[i]; }
    inline T& x() { return _c[0]; }
    inline T& y() { return _c[1]; }
    inline T& z() { return _c[2]; }
    inline T& w() { return _c[3]; }

    static inline Vector min() { return Vector(std::numeric_limits<T>::min()); }
    static inline Vector max() { return Vector(std::numeric_limits<T>::max()); }
    static Vector random() {
      Vector wtr;
      for(int i(0); i<d; i++)
        wtr[i] = Rand::nextFloat()-.5f;
      wtr.normalize();
      return wtr;
    }
  };
  template <int d,class T> inline Vector<d,T> operator*(const T& a,const Vector<d,T>& b) { return b*a; }

  typedef Vector<4,byte> Vector4b;
  typedef Vector<2,int> Vector2i;
  typedef Vector<3,int> Vector3i;
  typedef Vector<4,int> Vector4i;
  typedef Vector<2,float> Vector2f;
  typedef Vector<3,float> Vector3f;
  typedef Vector<4,float> Vector4f;

  template <int d,class T>
  Stream& operator<<(Stream &s,const Vector<d,T>& v) {
    s << '(';
    for(int i(0); i<d; i++) {
      s << v[i];
      if(i<d-1)
        s << ';';
    }
    s << ')';
    return s;
  }
}
