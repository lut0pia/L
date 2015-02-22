#ifndef DEF_L_Point
#define DEF_L_Point

#include <cmath>
#include <iostream>

/*
    // Returns a cartesian coord from a spherical one
    template <class T>
    Point3d<T> L_SpherToCart(Point3ds<T> const& p){
        return Point3d<T>(p.rho * sin(p.theta*M_PI/180) * cos(p.phi*M_PI/180),
                       p.rho * sin(p.theta*M_PI/180) * sin(p.phi*M_PI/180),
                       p.rho * cos(p.theta*M_PI/180));
    }

    // Returns a spherical coord from a cartesian one
    template <class T>
    Point3ds<T> L_CartToSpher(Point3d<T> const& p){
        Point3ds<T> wtr;
        wtr.rho = sqrt((p.x*p.x)+(p.y*p.y)+(p.z*p.z));
        wtr.theta = acos(p.z/wtr.rho)*180.0/M_PI;
        wtr.phi = acos(p.x/sqrt((p.x*p.x)+(p.y*p.y)))*180.0/M_PI;
        if(p.y<0)
            wtr.phi = L_PMod(360.0 - wtr.phi,360.0);
        if(wtr.theta<0.00001)wtr.theta = 0.00001; // Anti world-burning device
        return wtr;
    }
*/

namespace L {
  template <int d,class T>
  class Point {
    protected:
      T v[d];
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
      Point& operator/=(const Point& other) {
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

      T norm() {
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

      inline const T& operator[](size_t i) const {return v[i];}
      inline const T& x() const {return v[0];}
      inline const T& y() const {return v[1];}
      inline const T& z() const {return v[2];}
      inline const T& w() const {return v[3];}
      inline T& operator[](size_t i) {return v[i];}
      inline T& x() {return v[0];}
      inline T& y() {return v[1];}
      inline T& z() {return v[2];}
      inline T& w() {return v[3];}
  };
  typedef Point<2,int> Point2i;
  typedef Point<3,int> Point3i;
  typedef Point<3,float> Point3f;
}

#endif
