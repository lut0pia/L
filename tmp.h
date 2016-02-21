#ifndef DEF_L_tmp
#define DEF_L_tmp

#include "types.h"

namespace L {
  template <class T, T v> struct constant {static const T value = v;};
  template <int a, int b> struct static_pow : constant<int,a*static_pow<a,b-1>::value> {};
  template <int a> struct static_pow<a,0> : constant<int,1> {};
  template <uint f> struct flag : constant<uint,static_pow<2,f>::value> {};
  template <uint n> struct bitmask : constant<uint,~(~0<<n)> {};
}

#endif


