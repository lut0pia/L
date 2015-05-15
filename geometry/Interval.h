#ifndef DEF_L_Interval
#define DEF_L_Interval

#include <algorithm>

#include "../general.h"
#include "Point.h"

namespace L {
  template <int d,class T>
  class Interval {
    protected:
      Point<d,T> _min, _max;
    public:
      Interval() {
        clear();
      }
      Interval(const Point<d,T>& a, const Point<d,T>& b) : _min(a), _max(a) {
        add(b);
      }
      Interval(const Interval& a, const Interval& b) {
        for(int i(0); i<d; i++) {
          _min[i] = std::max(a._min[i],b._min[i]);
          _max[i] = std::min(a._max[i],b._max[i]);
        }
      }
      inline Interval operator*(const Interval& other) const {
        return Interval(*this,other);
      }
      inline Interval operator+(const Interval& other) const {
        Interval wtr;
        for(int i(0); i<d; i++) {
          wtr._min[i] = std::min(_min[i],other._min[i]);
          wtr._max[i] = std::max(_max[i],other._max[i]);
        }
        return wtr;
      }
      bool operator&&(const Interval& other) const {
        for(int i(0); i<d; i++) {
          if(std::max(_min[i],other._min[i]) > std::min(_max[i],other._max[i]))
            return false;
        }
        return true;
      }
      inline bool overlaps(const Interval& other) const {
        return (*this) && other;
      }

      void add(const Point<d,T>& p) {
        for(int i(0); i<d; i++) {
          if(_min[i] > p[i])
            _min[i] = p[i];
          if(_max[i] < p[i])
            _max[i] = p[i];
        }
      }
      bool contains(Point<d,T> p) const {
        if(empty()) return false;
        bool wtr(true);
        for(size_t i(0); i<d&&wtr; i++)
          if(!InBetween(p[i],_min[i],_max[i]))
            wtr = false;
        return wtr;
      }
      Point<d,T> closestTo(Point<d,T> p) const {
        for(size_t i(0); i<d; i++) {
          if(p[i]<_min[i])
            p[i] = _min[i];
          if(p[i]>_max[i])
            p[i] = _max[i];
        }
        return p;
      }
      Point<d,T> size() const {
        return _max - _min;
      }
      Point<d,T> center() const {
        return (_min + _max)/2;
      }
      bool empty() const {
        for(size_t i(0); i<d; i++)
          if(_min[i]>_max[i])
            return true;
        return false;
      }
      void clear() {
        _min = Point<d,T>::max();
        _max = Point<d,T>::min();
      }

      inline const Point<d,T>& min() const {return _min;}
      inline const Point<d,T>& max() const {return _max;}
  };
  typedef Interval<2,int> Interval2i;
  typedef Interval<3,int> Interval3i;
  typedef Interval<3,float> Interval3f;
}

#endif
