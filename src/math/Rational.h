#pragma once

#include "Integer.h"

namespace L {
  class Rational {
  private:
    Integer _a,_b; // a/b

    void simplify();

  public:
    inline Rational(long a) : _a(a),_b(1) {}
    inline Rational(const Integer& a = 0,const Integer& b = 1) : _a(a),_b(b) { simplify(); }
    Rational(const String&,uint32_t base = 10);

    inline Rational Rational::operator+(const Rational& o) const { return Rational((_a*o._b)+(o._a*_b),_b*o._b); }
    inline Rational Rational::operator-(const Rational& o) const { return Rational((_a*o._b)-(o._a*_b),_b*o._b); }
    inline Rational operator+() const { return *this; }
    inline Rational operator-() const{ return Rational(-_a,_b); }
    inline Rational Rational::operator*(const Rational& o) const { return Rational(*this) *= o; }
    inline Rational Rational::operator/(const Rational& o) const { return Rational(*this) /= o; }
    inline Rational Rational::operator%(const Rational& o) const { return *this-(Rational((*this/o).intval())*o); }

    inline Rational& operator ++(){ _a += _b; return *this; }
    inline Rational operator ++(int) { Rational wtr(*this); operator++(); return wtr; }
    inline Rational& operator --(){ _a -= _b; return *this; }
    inline Rational operator --(int) { Rational wtr(*this); operator--(); return wtr; }

    inline bool operator==(const Rational& o) const { return (_a*o._b==_b*o._a); }
    inline bool operator!=(const Rational& o) const { return !operator==(o); }
    inline bool operator>(const Rational& o) const { return (_a*o._b)>(o._a*_b); }
    inline bool operator<(const Rational& o) const { return (_a*o._b)<(o._a*_b); }
    inline bool operator>=(const Rational& o) const { return !operator<(o); }
    inline bool operator<=(const Rational& o) const { return !operator>(o); }

    inline Rational& operator +=(const Rational& o) { return *this = *this+o; }
    inline Rational& operator -=(const Rational& o) { return *this = *this-o; }
    inline Rational& operator *=(const Rational& o) { _a *= o._a; _b *= o._b; return *this; }
    inline Rational& operator /=(const Rational& o) { _a *= o._b; _b *= o._a; return *this; }
    inline Rational& operator %=(const Rational& o) { return *this = *this%o; }

    inline Integer intval() const { return _a/_b; }
    inline Rational abs() const { return Rational(_a.abs(),_b.abs()); }
    inline bool negative() const { return _a._negative!=_b._negative; }

    String toShortString() const;
    String toString(uint32_t base = 10) const;
  };

  inline Stream& operator<<(Stream &s,const Rational& v) { return s << v.toShortString(); }
}
