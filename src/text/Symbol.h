#pragma once

#include "../stream/Stream.h"

namespace L {
  //! String interning mechanism
  //! Should be thread-safe
  class Symbol {
  private:
    const char* _string;
  public:
    constexpr Symbol() : _string(nullptr) {}
    inline Symbol(const char* str) : Symbol(str, strlen(str)) {}
    Symbol(const char* str, size_t length);
    inline bool operator==(const Symbol& other) const { return _string==other._string; }
    inline bool operator!=(const Symbol& other) const { return _string!=other._string; }
    inline bool operator<(const Symbol& other) const { return strcmp(_string, other._string)<0; }
    inline operator const char*() const { return _string; }
    inline explicit operator bool() const { return _string!=nullptr; }
    friend inline Stream& operator<<(Stream& s, const Symbol& sym) { return s << sym._string; }
    friend inline Stream& operator<(Stream& s, const Symbol& sym) { return s << sym._string << '\n'; }
    friend inline Stream& operator>(Stream& s, Symbol& sym) { sym = s.word(); return s; }
    friend inline uint32_t hash(const Symbol& sym) { return uint32_t(uintptr_t(sym._string)); }
  };
}
