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
  
  inline uint32_t cas(volatile uint32_t* dst, uint32_t cmp, uint32_t exc) {
#if _MSC_VER
    return _InterlockedCompareExchange((volatile long*)dst, exc, cmp);
#elif __GNUC__
    return __sync_val_compare_and_swap(dst, cmp, exc);
#endif
  }
}
