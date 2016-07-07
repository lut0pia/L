#pragma once

#include "../types.h"

namespace L {
  // Describes the best integer type to convert T to (used for stringification)
  template <class T> struct inttype{ typedef int32_t type; };
  template <> struct inttype<int64_t>{ typedef int64_t type; };
  template <> struct inttype<float>{ typedef int64_t type; };
  template <> struct inttype<uint32_t>{ typedef uint32_t type; };
  template <> struct inttype<uint64_t>{ typedef uint64_t type; };

  template <int base = 10,class T>
  const char* ntos(T v,int pad = 0) { // Converts a number to a string
    static char buffer[128] = {0};
    char* wtr(buffer+127);
    // Check for negativity
    bool negative;
    if(v<0) {
      negative = true;
      v = -v;
    } else negative = false;
    // Check for point
    int point(0);
    while(T((typename inttype<T>::type)v)!=v) {
      v *= base;
      point++;
      if(point<0) break;
    }
    typename inttype<T>::type i(v); // Convert to int
    // Stringify
    if(!i) *--wtr = '0';
    else {
      while(pad>0 || i>0 || point>0) {
        char c('0'+(i%base));
        if(c>'9') c += 'a'-'9'-1;
        *--wtr = c;
        if(--point==0) *--wtr = '.';
        i /= base;
        pad--;
      }
      if(negative) *--wtr = '-'; // Add minus sign if negative
    }
    return wtr;
  }
  template <int base = 10> inline const char* ntos(double v,int pad = 0) { return ntos<base,float>((float)v,pad); }
  template <int base = 10,class T>
  T ston(const char* str) { // Converts a string to a number
    T wtr(0);
    bool negative(*str=='-' && *++str);
    int fract(0);
    char c;
    while((c = *str++)!='\0') {
      int v(0);
      if(c=='.') { fract = 1; continue; } else if(c>='0'&&c<='9') v = c-'0';
      else if(c>='a'&&c<='z') v = c-'a'+10;
      else if(c>='A'&&c<='Z') v = c-'A'+10;
      wtr *= base;
      wtr += v;
      if(fract) fract *= base;
    }
    if(fract) wtr /= fract;
    return (negative) ? -wtr : wtr;
  }
  template <int base = 2,class T>
  const T& log(const T& v) {
  }
  template <class T> inline constexpr T log(const T& x,const T& base) { return log(x)/log(base); }
  template <class T> inline constexpr const T& max(const T& a,const T& b) { return (a>b) ? a : b; }
  template <class T> inline constexpr const T& min(const T& a,const T& b) { return (a<b) ? a : b; }
  template <class T> inline constexpr const T& clamp(const T& v,const T& min,const T& max) { return (v<min) ? min : ((v>max) ? max : v); }
  template <class T> inline constexpr T abs(const T& n) { return (n<0) ? -n : n; }
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
