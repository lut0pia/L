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
  
  inline uint32_t cas(volatile uint32_t* dst, uint32_t cmp, uint32_t exc) {
#if _MSC_VER
    return _InterlockedCompareExchange(dst, exc, cmp);
#elif __GNUC__
    return __sync_val_compare_and_swap(dst, cmp, exc);
#endif
  }
}
