#pragma once

#include "../container/Array.h"
#include "../dynamic/Variable.h"
#include "../text/Symbol.h"

namespace L {
  namespace Script {
    class Context;
    typedef void(*Function)(Context&);
    typedef void(*Native)(Context&, const Array<Var>&);
    struct Local { uint32_t i; };
    struct RawSymbol { Symbol sym; };
    struct CodeFunction {
      typedef CodeFunction* Intermediate;
      Var code; uint32_t localCount;
    };
  }
}
