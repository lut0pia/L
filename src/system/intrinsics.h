#pragma once

#include "../types.h"

#if _MSC_VER
# include <Windows.h>
# include <intrin.h>
#endif

namespace L {
  inline uint32_t bsr(uint32_t v) {
#if _MSC_VER
    DWORD wtr;
    _BitScanReverse(&wtr,v);
    return wtr;
#elif __GNUC__
    return sizeof(v)*8-__builtin_clz(v);
#endif
  }
}
