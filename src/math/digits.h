#pragma once

#include <math.h>

namespace L {
  // Describes the best integer type to convert T to (used for stringification)
  template <class T> struct inttype { typedef int32_t type; };
  template <> struct inttype<int64_t> { typedef int64_t type; };
  template <> struct inttype<float> { typedef int64_t type; };
  template <> struct inttype<uint32_t> { typedef uint32_t type; };
  template <> struct inttype<uint64_t> { typedef uint64_t type; };

  inline char* ntos_buffer() {
    static char buffer[128] = {0};
    return buffer+127;
  }

  template <int base, class T>
  int ntos_point(T& v) {
    int wtr(0);
    while(T((typename inttype<T>::type)v)!=v) {
      v *= base;
      wtr++;
      if(wtr<0) break;
    }
    return wtr;
  }

  template <class T> inline bool ntos_valid(const T&) { return true; }
  inline bool ntos_valid(float v) { return !isnan(v); }

  template <int base = 10, class T>
  const char* ntos(T v, int pad = 0) { // Converts a number to a string
    if(!ntos_valid(v)) return "NaN"; // Check validity
    char* wtr(ntos_buffer()); // Get buffer (backwards writing)
    const bool negative(v<0); // Check negativity
    if(negative) v = -v;

    int point(ntos_point<base>(v)); // Check for point
    typename inttype<T>::type i(v); // Convert to int

    // Stringify
    if(!i) do { *--wtr = '0'; } while(--pad>0);
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
  template <int base = 10> inline const char* ntos(double v, int pad = 0) { return ntos<base, float>((float)v, pad); }

  template <int base = 10, class T>
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
}
