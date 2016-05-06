#pragma once

#include "FontBase.h"

namespace L {
  namespace Font {
    Base& get(const char* = "");
    void set(Base*, const char* = "");
  }
}
