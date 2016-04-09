#ifndef DEF_L_Font
#define DEF_L_Font

#include "FontBase.h"

namespace L {
  namespace Font {
    Base& get(const String& = "");
    void set(Base*, const String& = "");
  }
}

#endif

