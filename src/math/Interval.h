#pragma once

#include "../math/math.h"
#include "Matrix.h"
#include "Vector.h"

namespace L {
  template <int d,class T>
  class Interval {
  protected:
    Vector<d,T> _min,_max;
  public:
    inline Interval() = default;
    inline Interval(const Vector<d,T>& a) : _min(a),_max(a) {}
    inline Interval(const Vector<d,T>& a,const Vector<d,T>& b) : _min(a),_max(a) { add(b); }
    Interval(const Interval& a,const Interval& b) {
      for(int i(0); i<d; i++) {
        _min[i] = L::max(a._min[i],b._min[i]);
        _max[i] = L::min(a._max[i],b._max[i]);
      }
    }
    inline Interval& operator*=(const T& scalar) {
      _min *= scalar;
      _max *= scalar;
      return *this;
    }
    inline Interval operator*(const Interval& other) const {
      return Interval(*this,other);
    }
    inline Interval operator+(const Interval& other) const {
      Interval wtr;
      for(int i(0); i<d; i++) {
        wtr._min[i] = L::min(_min[i],other._min[i]);
        wtr._max[i] = L::max(_max[i],other._max[i]);
      }
      return wtr;
    }

    inline bool operator==(const Interval& other) const { return _min == other._min && _max == other._max; }
    inline bool operator!=(const Interval& other) const { return !(*this == other); }

    inline bool overlaps(const Interval& other) const {
      bool overlap(true);
      for(int i(0); i<d; i++)
        overlap &= (L::max(_min[i], other._min[i]) < L::min(_max[i], other._max[i]));
      return overlap;
    }

    void add(const Vector<d,T>& p) {
      for(int i(0); i<d; i++) {
        if(_min[i] > p[i])
          _min[i] = p[i];
        if(_max[i] < p[i])
          _max[i] = p[i];
      }
    }
    void extend(const T& scalar) {
      _min -= scalar;
      _max += scalar;
    }
    Interval extended(const T& scalar) const {
      Interval wtr(*this);
      wtr.extend(scalar);
      return wtr;
    }
    bool contains(Vector<d,T> p) const {
      bool containment(true);
      for(int i(0); i<d; i++)
        containment &= _min[i] < p[i] & p[i] < _max[i];
      return containment;
    }
    bool contains(Interval other) const{
      for(int i(0); i<d; i++)
        if(_min[i] > other._min[i] || other._max[i] > _max[i])
          return false;
      return true;
    }
    Vector<d,T> closestTo(Vector<d,T> p) const {
      for(size_t i(0); i<d; i++) {
        if(p[i]<_min[i])
          p[i] = _min[i];
        if(p[i]>_max[i])
          p[i] = _max[i];
      }
      return p;
    }
    inline Vector<d,T> size() const { return _max - _min; }
    inline Vector<d,T> center() const { return (_min + _max)/2; }
    inline T extent() const{ return size().product(); }
    bool empty() const {
      for(size_t i(0); i<d; i++)
        if(_min[i]>_max[i])
          return true;
      return false;
    }
    void clear() {
      _min = Vector<d,T>::max();
      _max = Vector<d,T>::min();
    }

    inline const Vector<d,T>& min() const { return _min; }
    inline const Vector<d,T>& max() const { return _max; }

    friend Stream& operator<<(Stream& s, const Interval& v) { return s << '(' << v.min() << ',' << v.max() << ')'; }
    friend Stream& operator<(Stream& s, const Interval& v) { return s < v.min() < v.max(); }
    friend Stream& operator>(Stream& s, Interval& v) { return s > v._min > v._max; }
  };
  typedef Interval<1,float> Interval1f;
  typedef Interval<2,int> Interval2i;
  typedef Interval<3,int> Interval3i;
  typedef Interval<2,float> Interval2f;
  typedef Interval<3,float> Interval3f;
}
