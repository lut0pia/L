#ifndef DEF_L_tmp
#define DEF_L_tmp

#include "types.h"

namespace L {
  // ENABLE IF
  template <bool B, class T = void>
  struct enable_if_c {
    typedef T type;
  };

  template <class T>
  struct enable_if_c<false, T> {};

  template <class Cond, class T = void>
  struct enable_if : public enable_if_c<Cond::value, T> {};

  // DISABLE IF
  template <class Cond, class T = void>
  struct disable_if : public enable_if_c<!Cond::value, T> {};


  template <uint a, uint b>
  struct static_pow {
    static const uint value = a*static_pow<a,b-1>::value;
  };

  template <uint a>
  struct static_pow<a,0> {
    static const uint value = 1;
  };

  template <uint f>
  struct flag {
    static const uint value = static_pow<2,f>::value;
  };
  template <uint n>
  struct bitmask {
    static const uint value = ~(~0<<n);
  };

  template <class T>
  struct alignment {
    static const uint size = sizeof(T);
    static const uint align = __alignof(T);
    static const uint pad = (align-(size%align));
    static const uint padded = size+pad;
  };
}

#endif


