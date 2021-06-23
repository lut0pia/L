#pragma once

#include <cstdint>

#if _MSC_VER
#include <intrin.h>
#endif

namespace L {
  inline uint32_t bsr(uint32_t v) {
#if _MSC_VER
    unsigned long wtr;
    _BitScanReverse(&wtr,v);
    return wtr;
#elif __GNUC__
    return sizeof(v)*8-__builtin_clz(v);
#endif
  }

  inline uint32_t bsr(uint64_t v){
#if _MSC_VER
    unsigned long wtr;
    _BitScanReverse64(&wtr, v);
    return wtr;
#elif __GNUC__
    return sizeof(v)*8-__builtin_clzll(v);
#endif
  }
}
