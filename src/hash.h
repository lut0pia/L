#pragma once

#include <cstddef>
#include <cstdint>

namespace L {
  inline uint32_t fnv1a(const char* str) {
    uint32_t wtr(2166136261);
    while(*str) {
      wtr ^= *str;
      wtr *= 16777619;
      str++;
    }
    return wtr;
  }
  inline uint32_t fnv1a(const char* data, size_t size) {
    uint32_t wtr(2166136261);
    while(size--) {
      wtr ^= *data;
      wtr *= 16777619;
      data++;
    }
    return wtr;
  }
  constexpr uint32_t FNV1A(const char* str, uint32_t r) { return (*str) ? FNV1A(str + 1, (r ^ *str) * 16777619) : r; }
  constexpr uint32_t FNV1A(const char* str) { return FNV1A(str, 2166136261); }

  template <class T> inline uint32_t hash(const T& v) { return fnv1a((const char*)&v, sizeof(v)); }
  inline uint32_t hash(const char* str) { return fnv1a(str); }
  inline uint32_t hash(uint32_t v) { return v + 1; }
  inline uint32_t hash() { return 2166136261; }

  template <class T> inline void hash_combine(uint32_t& h, const T& v) {
    h ^= hash(v) + 0x9e3779b9 + (h << 6) + (h >> 2);
  }

  template <class T, typename... A> inline uint32_t hash(const T& v, A&&... args) {
    uint32_t h = hash(args...);
    hash_combine(h, v);
    return h;
  }
}
