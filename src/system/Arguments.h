#pragma once

#include "../container/Array.h"
#include "../text/Symbol.h"

namespace L {
  namespace Arguments {
    void init(int argc, const char* argv[]);

    bool has(const Symbol& key);
    Symbol get(const Symbol& key);
    void get(const Symbol& key, Array<Symbol>& values);
  }
}
