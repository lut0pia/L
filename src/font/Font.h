#pragma once

#include "FontBase.h"
#include "../containers/Ref.h"

namespace L {
  namespace Font {
    Base& get(const char* = "");
    void set(const Ref<Base>&, const char* = "");
  }
}
