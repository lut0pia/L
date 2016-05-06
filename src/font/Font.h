#ifndef DEF_L_Font
#define DEF_L_Font

#include "FontBase.h"

namespace L {
  namespace Font {
    Base& get(const char* = "");
    void set(Base*, const char* = "");
  }
}

#endif

