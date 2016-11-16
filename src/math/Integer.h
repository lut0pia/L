#pragma once

#include "../containers/Array.h"
#include "../String.h"

namespace L {
  class Integer {
  private:
    typedef uint32_t type;
    static const size_t typebytes = sizeof(type);
    static const size_t typebits = typebytes*8;
    Array<type> _part; // Big-Endian
    bool _negative;
  public:
    inline Integer() : _negative(false) { _part.push(0); }
    inline Integer(const int& n) : _negative(n<0) {
      _part.push((negative()) ? -n : n);
    }
    Integer(const char*,int base = 10);

    inline Integer operator+(const Integer& other) const{ return Integer(*this) += other; }
    inline Integer operator-(const Integer& other) const{ return Integer(*this) -= other; }
    inline Integer operator+() const{ return *this; }
    Integer operator-() const;
    inline Integer operator*(const Integer& other) const{ return Integer(*this) *= other; }
    inline Integer operator/(const Integer& other) const{ return Integer(*this) /= other; }
    inline Integer operator%(const Integer& other) const{ return Integer(*this) %= other; }

    Integer& operator++();
    Integer operator++(int);
    Integer& operator--();
    Integer operator--(int);

    bool operator==(const Integer&) const;
    inline bool operator!=(const Integer& other) const { return !(*this==other); }
    bool operator<(const Integer&) const;
    inline bool operator>(const Integer& other) const{ return other<*this; }
    inline bool operator<=(const Integer& other) const{ return !(other<*this); }
    inline bool operator>=(const Integer& other) const{ return !(*this<other); }

    inline Integer operator<<(size_t n) const{ return Integer(*this) <<= n; }
    inline Integer operator>>(size_t n) const{ return Integer(*this) >>= n; }

    Integer& operator+=(const Integer&);
    Integer& operator-=(const Integer&);
    Integer& operator*=(const Integer&);
    Integer& operator/=(const Integer&);
    Integer& operator%=(const Integer&);
    Integer& operator<<=(size_t);
    Integer& operator>>=(size_t);

    Integer abs() const;
    String toShortString() const;
    String toString(long base = 10) const;

  private:
    type part(size_t) const;
    void part(size_t,type);
    void reset();
    void trim();
    inline size_t size() const { return _part.size(); }
    inline bool negative() const { return _negative; }

  public:
    static Integer lcd(const Integer& a,const Integer& b);
    static Integer gcd(Integer a,Integer b);
    static Integer pow(const Integer& a,Integer b);
    friend class Rational;
    friend Stream& operator<<(Stream&,const Integer&);
  };
}
