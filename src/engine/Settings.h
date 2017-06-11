#pragma once

#include "../text/Symbol.h"

namespace L {
  namespace Settings {
    void load_file(const char* path);
    int get_int(const Symbol&, int default_value = 0);
    float get_float(const Symbol&, float default_value = 0.f);
    const Symbol& get_symbol(const Symbol&, const Symbol& default_value = Symbol());
  }
}
