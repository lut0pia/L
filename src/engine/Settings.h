#pragma once

#include "../dynamic/Variable.h"
#include "../text/String.h"

namespace L {
  namespace Settings {
    void set(const Symbol&, const Var&);
    const int& get_int(const Symbol&, int default_value = 0);
    const float& get_float(const Symbol&, float default_value = 0.f);
    const String& get_string(const Symbol&, const String& default_value = String());
  }
}
