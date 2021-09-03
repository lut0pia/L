#pragma once

#include <cstddef>
#include <cstdint>

namespace L {
  class Rand {
    public:
      static uint64_t next();
      static uint8_t next_byte();
      static int next_int();
      static float next_float();

      static uint64_t next(uint64_t min, uint64_t max); // Returns a random unsigned integer between min and max
      static int next(int min, int max); // Returns a random integer between min and max
      static float next(float min, float max); // Returns a random float between min and max
      static void fill(uint8_t*,size_t);
      template <class T>
      inline static T next() {
        uint8_t wtr[sizeof(T)];
        fill(wtr,sizeof(T));
        return *((T*)&wtr);
      }

      static float gauss(uint32_t);
      static uint64_t next(uint64_t ave); // Random exp with an average n

      static uint64_t get_state();
      static void set_state(uint64_t s);
  };
}
