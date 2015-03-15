#ifndef DEF_L_Point
#define DEF_L_Point

#include <cmath>
#include <iostream>
#include <limits>
#include "../macros.h"

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
        for(size_t i(0); i<d; i++)
          v[i] = other[i];
      }
      Point(const T& scalar) {
        for(size_t i(0); i<d; i++)
          v[i] = scalar;
      }
      Point(const T& x, const T& y) : v {x,y} {}
      Point(const T& x, const T& y, const T& z) : v {x,y,z} {}
      Point(const T& x, const T& y, const T& z, const T& w) : v {x,y,z,w} {}
      Point(const Point<d-1,T>& p, const T& w) : v {p.v} {
        v[d-1] = w;
      }

      Point& operator+=(const Point& other) {
        for(size_t i(0); i<d; i++)
          v[i] += other.v[i];
        return *this;
      }
      Point& operator-=(const Point& other) {
        for(size_t i(0); i<d; i++)
          v[i] -= other.v[i];
        return *this;
      }
      Point& operator*=(const Point& other) {
        for(size_t i(0); i<d; i++)
          v[i] *= other.v[i];
        return *this;
      }
      Point& operator/=(const Point& other) const {
        for(size_t i(0); i<d; i++)
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
        for(size_t i(0); i<d; i++)
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
        for(size_t i(0); i<d; i++)
          if(v[i] != other.v[i])
            return false;
        return true;
      }
      bool operator!=(const Point& other) const {
        return !(*this == other);
      }
      bool operator<(const Point& other) const {
        for(size_t i(0); i<d; i++)
          if(v[i] < other.v[i])
            return true;
          else if(v[i] > other.v[i])
            return false;
        return false;
      }
      T norm() const {
        T wtr(0);
        for(size_t i(0); i<d; i++)
          wtr += v[i]*v[i];
        return sqrt(wtr);
      }
      void normalize() {
        T n(norm());
        for(size_t i(0); i<d; i++)
          v[i] /= n;
      }
      T dist(const Point& other) const {
        return (*this-other).norm();
      }
      Point cross(const Point& other) {
        return Point(y()*other.z() - z()*other.y(),
                     z()*other.x() - x()*other.z(),
                     x()*other.y() - y()*other.x());
      }
      T dot(const Point& other) const {
        T wtr(0);
        for(size_t i(0); i<d; i++)
          wtr += v[i]*other.v[i];
        return wtr;
      }
      bool increment(const Point& min,const Point& max, const T& delta = 1) {
        for(size_t i(0); i<d; i++) {
          v[i] += delta;
          if(v[i]>=max.v[i]) {
            v[i] = min.v[i];
            if(i==d-1)
              return false;
          } else break;
        }
        return true;
      }

      inline const T& operator[](size_t i) const {return v[i];}
      inline const T& x() const {return v[0];}
      inline const T& y() const {return v[1];}
      inline const T& z() const {return v[2];}
      inline const T& w() const {return v[3];}
      inline const byte* bytes() const {return b;}
      inline T& operator[](size_t i) {return v[i];}
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
  };
  typedef Point<2,int> Point2i;
  typedef Point<3,int> Point3i;
  typedef Point<3,float> Point3f;
}

#endif
