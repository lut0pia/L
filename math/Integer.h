#ifndef DEF_L_Integer
#define DEF_L_Integer

#include <cmath>

#include "../containers/Array.h"
#include "../stl/String.h"

namespace L {
  class Integer {
    private:
      typedef unsigned int type;
      static const size_t typebytes = sizeof(type);
      static const size_t typebits = typebytes*8;
      Array<type> _part; // Big-Endian
      bool _negative;
    public:
      Integer();
      Integer(const int&);
      Integer(const String&, int base = 10);

      Integer operator+(const Integer&) const;
      Integer operator-(const Integer&) const;
      Integer operator+() const;
      Integer operator-() const;
      Integer operator*(const Integer&) const;
      Integer operator/(const Integer&) const;
      Integer operator%(const Integer&) const;

      Integer& operator++();
      Integer operator++(int);
      Integer& operator--();
      Integer operator--(int);

      bool operator==(const Integer&) const;
      bool operator!=(const Integer&) const;
      bool operator>(const Integer&) const;
      bool operator<(const Integer&) const;
      bool operator>=(const Integer&) const;
      bool operator<=(const Integer&) const;

      Integer operator<<(const size_t&) const;
      Integer operator>>(const size_t&) const;

      Integer& operator+=(const Integer&);
      Integer& operator-=(const Integer&);
      Integer& operator*=(const Integer&);
      Integer& operator/=(const Integer&);
      Integer& operator%=(const Integer&);
      Integer& operator<<=(const size_t&);
      Integer& operator>>=(const size_t&);

      Integer abs() const;
      String toShortString() const;
      String toString(long base = 10) const;

    private:
      type part(size_t) const;
      void part(size_t,type);
      void reset();
      void trim();
      inline size_t size() const {return _part.size();}
      inline bool negative() const {return _negative;}

    public :
      static Integer lcd(const Integer& a, const Integer& b);
      static Integer gcd(Integer a, Integer b);
      static Integer pow(const Integer& a, Integer b);
      friend class Rational;
      friend Stream& operator<<(Stream&, const Integer&);
  };

  Stream& operator<<(Stream&, const Integer&);
}

#endif





