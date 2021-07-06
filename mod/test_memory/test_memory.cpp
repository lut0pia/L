#include <L/src/container/Array.h>
#include <L/src/dev/test.h>
#include <L/src/math/Rand.h>
#include <L/src/stream/StringStream.h>
#include <L/src/text/String.h>
#include <L/src/time/Timer.h>

using namespace L;

constexpr uintptr_t iterations = 1 << 20;
constexpr size_t max_block_size = 1 << 18;
constexpr size_t max_allocated_limit = 1 << 29;

void test_memory_module_init() {
  Test test_memory{};
  test_memory.name = "memory";
  test_memory.func = []() {
    bool success = true;

    Array<size_t*> blocks;

    size_t total_allocated = 0;
    size_t max_allocated = 0;
    size_t alloc_count = 0;
    size_t free_count = 0;

    Time alloc_time = 0;
    Time free_time = 0;

    log("test_memory: doing %d iterations", iterations);

    for(uintptr_t i = 0; i < iterations; i++) {
      if((iterations - i) > blocks.size() // Need to free everything by the end
         && total_allocated + max_block_size < max_allocated_limit // Need not to allocate over limit
         && (blocks.size() == 0 || Rand::next_float() < 0.5f)) {
        const size_t block_size = Rand::next(size_t(8), max_block_size);

        Timer alloc_timer;
        size_t* block = (size_t*)Memory::alloc(block_size);
        alloc_time += alloc_timer.since();
        alloc_count += 1;

        if(block == nullptr) {
          warning("test_memory: unable to allocate %d bytes at %d total allocated", block_size, total_allocated);
          continue;
        }
        *block = block_size;
        blocks.push(block);
        total_allocated += block_size;
        max_allocated = max(max_allocated, total_allocated);
      } else {
        const uintptr_t block_index = Rand::next(uintptr_t(0), blocks.size() - 1);
        size_t* block = blocks[block_index];
        blocks.erase_fast(block_index);
        total_allocated -= *block;

        Timer free_timer;
        Memory::free(block, *block);
        free_time += free_timer.since();
        free_count += 1;
      }
    }

    L_ASSERT(total_allocated == 0);
    L_ASSERT(blocks.size() == 0);
    L_ASSERT(max_allocated < max_allocated_limit);

    String alloc_time_avg_str = to_string(alloc_time / alloc_count);
    String free_time_avg_str = to_string(free_time / free_count);
    String alloc_time_total_str = to_string(alloc_time);
    String free_time_total_str = to_string(free_time);

    log("test_memory: max memory allocated: %d bytes", max_allocated);
    log("test_memory: %d alloc (avg %s, total %s)", alloc_count, alloc_time_avg_str.begin(), alloc_time_total_str.begin());
    log("test_memory: %d free (avg %s, total %s)", free_count, free_time_avg_str.begin(), free_time_total_str.begin());

    return success;
  };

  add_test(test_memory);
}
