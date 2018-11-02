#pragma once

#include "../dev/debug.h"
#include "Stream.h"

namespace L {
  template<class T> inline Stream& operator<(Stream& s, const T&) { error("Unimplemented text out"); return s; }
  template<class T> inline Stream& operator>(Stream& s, T&) { error("Unimplemented text in"); return s; }

#define L_SERIAL_NUMERIC(type)\
  inline Stream& operator<(Stream& s, type v) { return s << ntos<10,type>(v) << '\n'; }\
  inline Stream& operator>(Stream& s, type& v) { v = ston<10, type>(s.word()); return s; }\

  L_SERIAL_NUMERIC(int);
  L_SERIAL_NUMERIC(unsigned int);
  L_SERIAL_NUMERIC(long long);
  L_SERIAL_NUMERIC(unsigned long long);
  L_SERIAL_NUMERIC(float);
  L_SERIAL_NUMERIC(double);
#undef L_SERIAL_NUMERIC

  inline Stream& operator<(Stream& s, bool v) { return s << (v ? '1' : '0') << '\n'; }
  inline Stream& operator>(Stream& s, bool& v) { v = (!strcmp(s.word(), "1")); return s; }
}
