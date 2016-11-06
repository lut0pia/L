#pragma once

#include <cmath>

#include "Integer.h"
#include "../macros.h"

namespace L {
  class Rational {
    private:
      Integer a, b; // a/b

      void simplification();

    public:
      Rational(const long&);
      Rational(const Integer& = 0, const Integer& = 1);
      Rational(const String&, long base = 10);

      Rational operator+(const Rational&) const;
      Rational operator-(const Rational&) const;
      Rational operator+() const;
      Rational operator-() const;
      Rational operator*(const Rational&) const;
      Rational operator/(const Rational&) const;
      Rational operator%(const Rational&) const;

      Rational& operator ++();
      Rational operator ++(int);
      Rational& operator --();
      Rational operator --(int);

      bool operator==(const Rational&) const;
      bool operator!=(const Rational&) const;
      bool operator>(const Rational&) const;
      bool operator<(const Rational&) const;
      bool operator>=(const Rational&) const;
      bool operator<=(const Rational&) const;

      Rational& operator +=(const Rational&);
      Rational& operator -=(const Rational&);
      Rational& operator *=(const Rational&);
      Rational& operator /=(const Rational&);
      Rational& operator %=(const Rational&);

      const Integer& gA() const;
      const Integer& gB() const;
      Integer intval() const;
      Rational abs() const;
      bool negative() const;

      String toShortString() const;
      String toString(long base = 10) const;
  };

  inline Stream& operator<<(Stream &s, const Rational& v) {
    return s << v.toShortString();
  }
}
