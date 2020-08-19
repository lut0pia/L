#pragma once

namespace L {
  template<size_t S>
  class Bitfield {
  protected:
    static constexpr size_t N = S / 32 + 1;
    uint32_t _data[N];

  public:
    constexpr Bitfield() = default;
    constexpr Bitfield(uint32_t d) : _data {d} {}

    inline Bitfield& operator&=(const Bitfield& o) { for(uintptr_t i = 0; i < N; i++) { _data[i] &= o._data[i]; } return *this; }
    inline Bitfield& operator|=(const Bitfield& o) { for(uintptr_t i = 0; i < N; i++) { _data[i] |= o._data[i]; } return *this; }
    inline Bitfield& operator^=(const Bitfield& o) { for(uintptr_t i = 0; i < N; i++) { _data[i] ^= o._data[i]; } return *this; }

    inline bool operator[](uintptr_t i) const { return (_data[i / 32] & (1 << (i % 32))) != 0; }

    inline void set(uintptr_t i) { _data[i / 32] |= (1 << (i % 32)); }
    inline void unset(uintptr_t i) { _data[i / 32] &= ~(1 << (i % 32)); }
    inline void clear(uint8_t v = 0) { memset(_data, v, sizeof(_data)); }
  };
}
