#pragma once

#include "types.h"

namespace L{
  inline uint32_t fnv1a(const char* str){
    uint32_t wtr(2166136261);
    while(*str) {
      wtr ^= *str;
      wtr *= 16777619;
      str++;
    }
    return wtr;
  }
  constexpr uint32_t FNV1A(const char* str,uint32_t r){
    return (*str) ? FNV1A(str+1,(r^*str)*16777619) : r;
  }
  constexpr uint32_t FNV1A(const char* str){
    return FNV1A(str,2166136261);
  }
}