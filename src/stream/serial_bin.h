#pragma once

#include "../dev/debug.h"
#include "Stream.h"

namespace L {
  template<class T> inline Stream& operator<=(Stream& s, const T& v) { s.write(&v, sizeof(v)); return s; }
  template<class T> inline Stream& operator>=(Stream& s, T& v) { s.read(&v, sizeof(v)); return s; }
}
