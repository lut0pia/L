#ifndef DEF_L_Interval
#define DEF_L_Interval

#include "../math/math.h"
#include "Matrix.h"
#include "Vector.h"

namespace L {
  template <int d,class T>
  class Interval {
    protected:
      Vector<d,T> _min, _max;
    public:
      inline Interval() = default;
      inline Interval(const Vector<d,T>& a) : _min(a), _max(a) {}
      inline Interval(const Vector<d,T>& a, const Vector<d,T>& b) : _min(a), _max(a) {add(b);}
      Interval(const Interval& a, const Interval& b) {
        for(int i(0); i<d; i++) {
          _min[i] = L::max(a._min[i],b._min[i]);
          _max[i] = L::min(a._max[i],b._max[i]);
        }
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
      Vector<d,T> corner(int c) const {
        Vector<d,T> wtr;
        for(int i(0); i<d; i++)
          wtr[i] = (c&(1<<i))?_min[i]:_max[i];
        return wtr;
      }
      Interval transformed(const Matrix<d+1,d+1,T>& matrix) const {
        if(!empty()) { // Empty should always return empty
          Interval wtr;
          Vector<d,T> p;
          for(int c(0); c<(1<<d); c++) { // Cycle through corners of the interval
            for(int i(0); i<d; i++)
              p[i] = (c&(1<<i))?_min[i]:_max[i];
            if(c) wtr.add(Vector<d,T>(matrix*p));
            else wtr = Interval(Vector<d,T>(matrix*p)); // First
          }
          return wtr;
        } else return *this;
      }
      bool operator&&(const Interval& other) const {
        for(int i(0); i<d; i++) {
          if(L::max(_min[i],other._min[i]) > L::min(_max[i],other._max[i]))
            return false;
        }
        return true;
      }
      inline bool overlaps(const Interval& other) const {
        return (*this) && other;
      }

      void add(const Vector<d,T>& p) {
        for(int i(0); i<d; i++) {
          if(_min[i] > p[i])
            _min[i] = p[i];
          if(_max[i] < p[i])
            _max[i] = p[i];
        }
      }
      bool contains(Vector<d,T> p) const {
        if(empty()) return false;
        bool wtr(true);
        for(size_t i(0); i<d&&wtr; i++)
          if(!(_min[i] <= p[i] && p[i] <= _max[i]))
            wtr = false;
        return wtr;
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
      Vector<d,T> size() const {
        return _max - _min;
      }
      Vector<d,T> center() const {
        return (_min + _max)/2;
      }
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

      inline const Vector<d,T>& min() const {return _min;}
      inline const Vector<d,T>& max() const {return _max;}
  };
  typedef Interval<1,float> Interval1f;
  typedef Interval<2,int> Interval2i;
  typedef Interval<3,int> Interval3i;
  typedef Interval<2,float> Interval2f;
  typedef Interval<3,float> Interval3f;

  template <int d,class T>
  Stream& operator<<(Stream &s, const Interval<d,T>& v) {
    return s << '(' << v.min() << ',' << v.max() << ')';
  }
}

#endif
