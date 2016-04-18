#pragma once

#include "types.h"

namespace L {
  template <class T, T v> struct constant {static const T value = v;};
  template <int a, int b> struct static_pow : constant<int,a*static_pow<a,b-1>::value> {};
  template <int a> struct static_pow<a,0> : constant<int,1> {};
  template <uint32_t f> struct flag : constant<uint32_t,static_pow<2,f>::value> {};
  template <uint32_t n> struct bitmask : constant<uint32_t,~(~0<<n)> {};
}
