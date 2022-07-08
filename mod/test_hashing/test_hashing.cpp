#include <L/src/container/Array.h>
#include <L/src/container/KeyValue.h>
#include <L/src/dev/debug.h>
#include <L/src/dev/test.h>
#include <L/src/hash.h>
#include <L/src/math/Rand.h>
#include <L/src/math/math.h>
#include <L/src/stream/StringStream.h>
#include <L/src/text/String.h>
#include <L/src/time/Time.h>
#include <cstring>

using namespace L;

constexpr uintptr_t max_iterations = 1 << 19;
constexpr uint32_t max_index = 20;

typedef KeyValue<String, uint32_t> HashPair;
static Array<HashPair> hashes;
static Array<size_t> collisions;
static size_t total_collisions;
static Array<Time> times;

static const HashPair* find_hash(uint32_t value) {
  for(const HashPair& pair : hashes) {
    if(pair.value() == value) {
      return &pair;
    }
  }
  return nullptr;
}

static String make_word(size_t size) {
  String str;
  while(size-- != 0) {
    str.push((char)Rand::next(32, 127));
  }
  return str;
}

static size_t size_for_index(uint32_t index) {
  return 1ull << index;
}

static size_t iterations_for_index(size_t index) {
  return min<size_t>(
    max<size_t>(1ull, max_iterations >> index),
    1ull << (min<size_t>(7ull, size_for_index(index)) * 8));
}

static void hash_word(uint32_t index) {
  const size_t size = size_for_index(index);
  const String word = make_word(size);
  const Time start_time = Time::now();
  const uint32_t value = hash(word);
  times[index] += Time::now() - start_time;

  bool add_hash = true;
  if(const HashPair* found_hash = find_hash(value)) {
    if(found_hash->key() == word) {
      add_hash = false;
    } else {
      collisions[index] += 1;
      total_collisions += 1;
    }
  }

  if(add_hash) {
    hashes.push(HashPair(word, value));
  }
}

void test_hashing_module_init() {
  Test test_hashing{};
  test_hashing.name = "hashing";
  test_hashing.func = []() {
    bool success = true;
    total_collisions = 0;

    for(size_t index = 0; index < max_index; index++) {
      const size_t word_size = size_for_index(index);
      const size_t iterations = iterations_for_index(index);
      collisions.push(0);
      times.push(0);

      for(size_t i = 0; i < iterations; i++) {
        hash_word(index);
      }

      const String time_per_mb = to_string((times[index] * int64_t(1ll << 20)) / (word_size * iterations));
      log("test_hashing: %d chars | %d cols / %d iters | %s/MB",
        word_size, collisions[index], iterations, time_per_mb.begin());
    }
    log("test_hashing: total collisions: %d", total_collisions);
    return success;
  };

  add_test(test_hashing);
}
