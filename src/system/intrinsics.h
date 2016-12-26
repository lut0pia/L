#pragma once

#include "../types.h"

#if _MSC_VER
# include <Windows.h>
# include <intrin.h>
#endif

namespace L {
#if _MSC_VER
  inline uint32_t bsr(uint32_t v) {
    DWORD wtr;
    _BitScanReverse(&wtr,v);
    return wtr;
  }
#endif
}
