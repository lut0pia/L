#pragma once

#include <cmath>
#include "../types.h"
#include "../system/intrinsics.h"

namespace L {
  // Logs
  template <class T> T log2(const T& x) {}
  template <class T> inline constexpr T log(const T& x, const T& base) { return log2(x)/log2(base); }
  inline uint32_t log2(uint32_t x) { return bsr(x); }
  inline uint64_t log2(uint64_t x) { return bsr(x); }
  inline uint32_t clog2(uint32_t x) { return bsr((x << 1) - 1); }
  inline uint64_t clog2(uint64_t x) { return bsr((x << 1) - 1); }

  inline uint32_t upperpow2(uint32_t v) { return 2 << clog2(v); }
  inline uint64_t upperpow2(uint64_t v) { return 2 << clog2(v); }

  template <class T> inline constexpr const T& max(const T& a, const T& b) { return (a>b) ? a : b; }
  template <class T> inline constexpr const T& min(const T& a, const T& b) { return (a<b) ? a : b; }
  template <class T> inline constexpr const T& clamp(const T& v, const T& min, const T& max) { return (v<min) ? min : ((v>max) ? max : v); }
  template <class T> inline constexpr T abs(const T& n) { return (n<0) ? -n : n; }
  template <class T> inline constexpr T sqr(const T& x) { return x*x; }
  inline constexpr uint32_t bitmask(uint32_t bits) { return ~(~0<<bits); }

  template <class T> T pmod(T x, const T& m) {
    while(x>=m)x -= m;
    while(x<0)x += m;
    return x;
  }
  template <> inline int pmod(int x, const int& m) {
    int wtr(x%m);
    return (wtr<0) ? wtr+m : wtr;
  }

  template <class T> T lcd(const T& a, const T& b) {
    T c(a), d(b);
    while(c!=d) {
      if(c<d) c += a;
      else d += b;
    }
    return c;
  }
  template <class T> T gcd(T a, T b) {
    if(a<b) swap(a, b);
    if(b!=0) {
      T c;
      while((b!=1)&&(a%b!=0)) {
        c = a%b;
        a = b;
        b = c;
      }
      return b;
    } else return a;
  }
  template <class T> T pow(const T& a, T b) {
    T wtr(1);
    while(--b>=0) wtr *= a;
    return wtr;
  }
  template <> inline float pow(const float& a, float b) { return ::pow(a, b); }
  template <> inline double pow(const double& a, double b) { return ::pow(a, b); }
}
