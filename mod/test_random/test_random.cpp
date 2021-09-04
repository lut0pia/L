#include <L/src/dev/debug.h>
#include <L/src/dev/test.h>
#include <L/src/math/Rand.h>
#include <L/src/math/math.h>
#include <cstring>

using namespace L;

constexpr uintptr_t max_width = 1 << 8;
constexpr uintptr_t iterations = max_width * 128;

void test_random_module_init() {
  Test test_random{};
  test_random.name = "random";
  test_random.func = []() {
    uint64_t dist[max_width] = {};
    float max_variance = 0.f;
    bool success = true;

    for(uintptr_t width = 2; width < max_width; width++) {
      memset(dist, 0, sizeof(uint64_t) * width);

      for(uintptr_t i = 0; i < iterations; i++) {
        uint64_t value = Rand::next(width);
        if(value >= width) {
          warning("test_random: bounded uint64_t next returned number out-of-bounds");
          continue;
        }

        dist[value] += 1;
      }

      uint64_t min_value = dist[0];
      uint64_t max_value = dist[0];
      for(uintptr_t i = 0; i < width; i++) {
        min_value = min(min_value, dist[i]);
        max_value = max(max_value, dist[i]);
      }

      const float variance = 1.f - (float(min_value) / float(max_value));
      if(variance > max_variance) {
        log("test_random: width=%d min=%d max=%d var=%.2f%%", width, min_value, max_value, variance * 100.f);
        max_variance = variance;
      }
    }
    return success;
  };

  add_test(test_random);
}
