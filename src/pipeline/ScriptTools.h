#pragma once

#include "../script/script.h"

namespace L {
  namespace ScriptTools {
    //! Optimize script bytecode without modifying end behaviour
    void optimize(Script&);
    void print_disassembly(const Script&, Stream&);
  };
}
