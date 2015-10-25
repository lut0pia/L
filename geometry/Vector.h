#ifndef DEF_L_Point
#define DEF_L_Point

#include <cmath>
#include <limits>
#include "../types.h"
#include "../Rand.h"
#include "../streams/Stream.h"

namespace L {
  template <int d,class T>
  class Vector {
    protected:
      T _c[d];
    public:
      inline Vector() {}
      template <class R>
      inline Vector(const Vector<d,R>& other) {
        for(int i(0); i<d; i++)
          _c[i] = other[i];
      }
      inline Vector(const T& v) {
        for(int i(0); i<d; i++)
          _c[i] = v;
      }
      template <typename... Args>
      inline Vector(const T& v, Args&&... args) {
        _c[0] = v;
        new(((T*)this)+1)Vector<d-1,T>(args...);
      }
      Vector(const Vector<d-1,T>& p, const T& w = 1.0) {
        for(int i(0); i<d-1; i++)
          _c[i] = p[i];
        _c[d-1] = w;
      }
      Vector(const Vector<d+1,T>& p) {
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
      inline Vector operator+(const Vector& other) const {Vector wtr(*this); wtr += other; return wtr;}
      inline Vector operator-(const Vector& other) const {Vector wtr(*this); wtr -= other; return wtr;}
      Vector operator-() const {
        Vector wtr;
        for(int i(0); i<d; i++)
          wtr._c[i] = -_c[i];
        return wtr;
      }
      inline Vector operator*(const Vector& other) const {Vector wtr(*this); wtr *= other; return wtr;}
      inline Vector operator/(const Vector& other) const {Vector wtr(*this); wtr /= other; return wtr;}

      bool operator==(const Vector& other) const {
        for(int i(0); i<d; i++)
          if(_c[i] != other._c[i])
            return false;
        return true;
      }
      bool operator!=(const Vector& other) const {
        return !(*this == other);
      }
      bool operator<(const Vector& other) const {
        for(int i(0); i<d; i++)
          if(_c[i] < other._c[i])
            return true;
          else if(_c[i] > other._c[i])
            return false;
        return false;
      }
      T normSquared() const {
        T wtr(0);
        for(int i(0); i<d; i++)
          wtr += _c[i]*_c[i];
        return wtr;
      }
      inline T norm() const {
        return std::sqrt(normSquared());
      }
      T manhattan() const {
        T wtr(0);
        for(int i(0); i<d; i++)
          wtr += abs(_c[i]);
        return wtr;
      }
      Vector& normalize() {
        T n(norm());
        for(int i(0); i<d; i++)
          _c[i] /= n;
        return *this;
      }
      inline T dist(const Vector& other) const {
        return (*this-other).norm();
      }
      inline T distSquared(const Vector& other) const {
        return (*this-other).normSquared();
      }
      Vector cross(const Vector& other) {
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
      bool increment(const Vector& min, const Vector& max, const Vector& delta = 1) {
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

      inline const T& operator[](int i) const {return _c[i];}
      inline const T& x() const {return _c[0];}
      inline const T& y() const {return _c[1];}
      inline const T& z() const {return _c[2];}
      inline const T& w() const {return _c[3];}
      inline const byte* bytes() const {return (const byte*)&_c;}
      inline const T* array() const {return _c;}
      inline T& operator[](int i) {return _c[i];}
      inline T& x() {return _c[0];}
      inline T& y() {return _c[1];}
      inline T& z() {return _c[2];}
      inline T& w() {return _c[3];}

      static Vector min() {
        Vector wtr;
        for(int i(0); i<d; i++)
          wtr[i] = std::numeric_limits<T>::min();
        return wtr;
      }
      static Vector max() {
        Vector wtr;
        for(int i(0); i<d; i++)
          wtr[i] = std::numeric_limits<T>::max();
        return wtr;
      }
      static Vector random() {
        Vector wtr;
        for(int i(0); i<d; i++)
          wtr[i] = Rand::nextFloat()-.5f;
        wtr.normalize();
        return wtr;
      }
  };

  typedef Vector<4,byte> Vector4b;
  typedef Vector<2,int> Vector2i;
  typedef Vector<3,int> Vector3i;
  typedef Vector<2,float> Vector2f;
  typedef Vector<3,float> Vector3f;
  typedef Vector<4,float> Vector4f;

  template <int d,class T>
  Stream& operator<<(Stream &s, const Vector<d,T>& v) {
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

#endif
