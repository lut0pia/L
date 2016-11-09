#pragma once

#include "../types.h"

namespace L {
  template <int base = 2,class T>
  const T& log(const T& v) {
  }
  template <class T> inline constexpr T log(const T& x,const T& base) { return log(x)/log(base); }
  template <class T> inline constexpr const T& max(const T& a,const T& b) { return (a>b) ? a : b; }
  template <class T> inline constexpr const T& min(const T& a,const T& b) { return (a<b) ? a : b; }
  template <class T> inline constexpr const T& clamp(const T& v,const T& min,const T& max) { return (v<min) ? min : ((v>max) ? max : v); }
  template <class T> inline constexpr T abs(const T& n) { return (n<0) ? -n : n; }
  template <class T> inline constexpr T sqr(const T& x){ return x*x; }
  inline constexpr uint32_t bitmask(uint32_t bits){ return ~(~0<<bits); }

  template <class T> T pmod(T x,const T& m) {
    while(x>=m)x -= m;
    while(x<0)x += m;
    return x;
  }
  template <> inline int pmod(int x,const int& m) {
    int wtr(x%m);
    return (wtr<0) ? wtr+m : wtr;
  }

  template <typename T>
  constexpr T orright(T v,int i){
    return (i) ? (v|orright(v>>1,i-1)) : 0;
  }
  template <typename T>
  constexpr T upperpow2(T v){ // Returns next upper power of 2
    return orright(v-1,sizeof(T)*8)+1;
  }
}
