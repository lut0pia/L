#pragma once

#include <cmath>
#include <cstdint>

namespace L {
  // Describes the best integer type to convert T to (used for stringification)
  template <class T> struct inttype { typedef int32_t type; };
  template <> struct inttype<int64_t> { typedef int64_t type; };
  template <> struct inttype<float> { typedef int64_t type; };
  template <> struct inttype<uint32_t> { typedef uint32_t type; };
  template <> struct inttype<uint64_t> { typedef uint64_t type; };

  template <class T> inline void digits_negate(T& v) { v = -v; }
  inline void digits_negate(uint32_t&) {}
  inline void digits_negate(uint64_t&) {}

  inline char* ntos_buffer() {
    static char buffer[128] = {0};
    return buffer+127;
  }

  template <int base, class T>
  int ntos_point(T& v) {
    int wtr = 0;
    while(T((typename inttype<T>::type)v) != v && !ntos_inf(v)) {
      v *= base;
      wtr++;
      if(wtr < 0) {
        break;
      }
    }
    return wtr;
  }

  template <class T> inline bool ntos_inf(const T&) { return false; }
  inline bool ntos_inf(float v) { return std::isinf(v); }

  template <class T> inline bool ntos_valid(const T&) { return true; }
  inline bool ntos_valid(float v) { return !std::isnan(v); }

  template <int base = 10, class T>
  const char* ntos(T v, int pad = 0) { // Converts a number to a string
    if(!ntos_valid(v)) { // Check validity
      return "NaN";
    }
    const bool negative = v < 0; // Check negativity
    if(negative) {
      digits_negate(v);
    }

    int point = ntos_point<base>(v); // Check for point
    if(ntos_inf(v)) { // Check infinity
      return "inf";
    }

    auto i = (typename inttype<T>::type)v; // Convert to int
    char* wtr = ntos_buffer(); // Get buffer (backwards writing)

    // Stringify
    if(!i) {
      do {
        *--wtr = '0';
      } while(--pad > 0);
    } else {
      while(pad > 0 || i > 0 || point > 0) {
        char c = '0' + (i % base);
        if(c > '9') {
          c += 'a' - '9' - 1;
        }
        *--wtr = c;
        if(--point == 0) {
          *--wtr = '.';
        }
        i /= base;
        pad--;
      }
      if(negative) {
        *--wtr = '-'; // Add minus sign if negative
      }
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
    if(negative) digits_negate(wtr);
    return wtr;
  }
}
