#pragma once

#include "../container/Array.h"
#include "../dynamic/Variable.h"
#include "../text/Symbol.h"
#include "../types.h"

namespace L {
  namespace Script {
    class Context;
    typedef void(*Function)(Context&);
    typedef void(*Native)(Context&, const Array<Var>&);
    typedef struct { uint32_t i; } Local;
    typedef struct { Symbol sym; } RawSymbol;
    typedef struct { Var code; uint32_t localCount; } CodeFunction;
  }
}
