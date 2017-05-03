#pragma once

#include "../streams/Stream.h"
#include "../types.h"

namespace L {
  class Fixed {
  private:
    int32_t _raw;
  public:
    static const int bits = sizeof(int32_t)*8;
    static const int halfbits = bits/2;
    static const int mul = 1<<halfbits;

    inline Fixed() {}
    inline Fixed(int i) : _raw(i<<halfbits) {}
    inline Fixed(float f) : Fixed((double)f) {}
    Fixed(double);

    template <class T> inline Fixed operator+(const T& other) { return Fixed(*this) += other; }
    template <class T> inline Fixed operator-(const T& other) { return Fixed(*this) -= other; }
    template <class T> inline Fixed operator*(const T& other) { return Fixed(*this) *= other; }
    template <class T> inline Fixed operator/(const T& other) { return Fixed(*this) /= other; }
    template <class T> inline Fixed operator%(const T& other) { return Fixed(*this) %= other; }
    inline Fixed operator-() { Fixed wtr; wtr._raw = -_raw; return wtr; }

    inline Fixed& operator+=(const Fixed& other) { _raw += other._raw; return *this; }
    inline Fixed& operator-=(const Fixed& other) { _raw -= other._raw; return *this; }
    inline Fixed& operator*=(const Fixed& other) {
      _raw = (int64_t(_raw)*int64_t(other._raw))>>halfbits;
      return *this;
    }
    inline Fixed& operator/=(const Fixed& other) {
      _raw = (int64_t(_raw)<<halfbits)/int64_t(other._raw);
      return *this;
    }
    inline Fixed& operator%=(const Fixed& other) { _raw %= other._raw; return *this; }
    template <class T> inline Fixed& operator+=(const T& other) { _raw += (int)(other*0x10000); return *this; }
    template <class T> inline Fixed& operator-=(const T& other) { _raw -= (int)(other*0x10000); return *this; }
    template <class T> inline Fixed& operator*=(const T& other) { _raw *= other; return *this; }
    template <class T> inline Fixed& operator/=(const T& other) { _raw /= other; return *this; }

    inline int raw() const { return _raw; }

    inline operator int() { return _raw>>halfbits; }
    inline operator float() { return (float)_raw/mul; }
    inline operator double() { return (double)_raw/mul; }
  };
  Stream& operator<<(Stream&, const Fixed&);
}
