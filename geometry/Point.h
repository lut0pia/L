#ifndef DEF_L_Point
#define DEF_L_Point

#include <cmath>
#include <limits>
#include "../types.h"
#include "../Rand.h"
#include "../streams/Stream.h"

namespace L {
  template <int d,class T>
  class Point {
    protected:
      union {
        T v[d];
        byte b[d*sizeof(T)];
      };
    public:
      Point() {}
      template <class R>
      Point(const Point<d,R>& other) {
        for(int i(0); i<d; i++)
          v[i] = other[i];
      }
      Point(const T& scalar) {
        for(int i(0); i<d; i++)
          v[i] = scalar;
      }
      Point(const T& x, const T& y) {
        v[0] = x;
        v[1] = y;
      }
      Point(const T& x, const T& y, const T& z) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
      }
      Point(const T& x, const T& y, const T& z, const T& w) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
        v[3] = w;
      }
      Point(const Point<d-1,T>& p, const T& w = 1.0) {
        for(int i(0); i<d-1; i++)
          v[i] = p[i];
        v[d-1] = w;
      }
      Point(const Point<d+1,T>& p) {
        for(int i(0); i<d; i++)
          v[i] = p[i];
      }

      Point& operator+=(const Point& other) {
        for(int i(0); i<d; i++)
          v[i] += other.v[i];
        return *this;
      }
      Point& operator-=(const Point& other) {
        for(int i(0); i<d; i++)
          v[i] -= other.v[i];
        return *this;
      }
      Point& operator*=(const Point& other) {
        for(int i(0); i<d; i++)
          v[i] *= other.v[i];
        return *this;
      }
      Point& operator/=(const Point& other) {
        for(int i(0); i<d; i++)
          v[i] /= other.v[i];
        return *this;
      }
      Point operator+(const Point& other) const {
        Point wtr(*this);
        wtr += other;
        return wtr;
      }
      Point operator-(const Point& other) const {
        Point wtr(*this);
        wtr -= other;
        return wtr;
      }
      Point operator-() const {
        Point wtr;
        for(int i(0); i<d; i++)
          wtr.v[i] = -v[i];
        return wtr;
      }
      Point operator*(const Point& other) const {
        Point wtr(*this);
        wtr *= other;
        return wtr;
      }
      Point operator/(const Point& other) const {
        Point wtr(*this);
        wtr /= other;
        return wtr;
      }

      bool operator==(const Point& other) const {
        for(int i(0); i<d; i++)
          if(v[i] != other.v[i])
            return false;
        return true;
      }
      bool operator!=(const Point& other) const {
        return !(*this == other);
      }
      bool operator<(const Point& other) const {
        for(int i(0); i<d; i++)
          if(v[i] < other.v[i])
            return true;
          else if(v[i] > other.v[i])
            return false;
        return false;
      }
      T normSquared() const {
        T wtr(0);
        for(int i(0); i<d; i++)
          wtr += v[i]*v[i];
        return wtr;
      }
      inline T norm() const {
        return std::sqrt(normSquared());
      }
      T manhattan() const {
        T wtr(0);
        for(int i(0); i<d; i++)
          wtr += abs(v[i]);
        return wtr;
      }
      Point& normalize() {
        T n(norm());
        for(int i(0); i<d; i++)
          v[i] /= n;
        return *this;
      }
      inline T dist(const Point& other) const {
        return (*this-other).norm();
      }
      inline T distSquared(const Point& other) const {
        return (*this-other).normSquared();
      }
      Point cross(const Point& other) {
        return Point(y()*other.z() - z()*other.y(),
                     z()*other.x() - x()*other.z(),
                     x()*other.y() - y()*other.x());
      }
      T dot(const Point& other) const {
        T wtr(0);
        for(int i(0); i<d; i++)
          wtr += v[i]*other.v[i];
        return wtr;
      }
      T product() const {
        T wtr(1);
        for(int i(0); i<d; i++)
          wtr *= v[i];
        return wtr;
      }
      bool increment(const Point& min, const Point& max, const Point& delta = 1) {
        for(int i(0); i<d; i++) {
          v[i] += delta[i];
          if(v[i]>=max.v[i]) {
            v[i] = min.v[i];
            if(i==d-1)
              return false;
          } else break;
        }
        return true;
      }

      inline const T& operator[](int i) const {return v[i];}
      inline const T& x() const {return v[0];}
      inline const T& y() const {return v[1];}
      inline const T& z() const {return v[2];}
      inline const T& w() const {return v[3];}
      inline const byte* bytes() const {return b;}
      inline const T* array() const {return v;}
      inline T& operator[](int i) {return v[i];}
      inline T& x() {return v[0];}
      inline T& y() {return v[1];}
      inline T& z() {return v[2];}
      inline T& w() {return v[3];}

      static Point min() {
        Point wtr;
        for(int i(0); i<d; i++)
          wtr[i] = std::numeric_limits<T>::min();
        return wtr;
      }
      static Point max() {
        Point wtr;
        for(int i(0); i<d; i++)
          wtr[i] = std::numeric_limits<T>::max();
        return wtr;
      }
      static Point random() {
        Point wtr;
        for(int i(0); i<d; i++)
          wtr[i] = Rand::nextFloat()-.5f;
        wtr.normalize();
        return wtr;
      }
  };

  typedef Point<2,int> Point2i;
  typedef Point<3,int> Point3i;
  typedef Point<2,float> Point2f;
  typedef Point<3,float> Point3f;
  typedef Point<4,float> Point4f;

  template <int d,class T>
  Stream& operator<<(Stream &s, const Point<d,T>& v) {
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
