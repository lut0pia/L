#include "format.h"

using namespace L;

String L::format_memory_amount(size_t bytes) {
  const char* suffixes[] = {
    "B",
    "KB",
    "MB",
    "GB",
    "TB",
    "PB",
    "EB",
    "ZB",
  };

  uintptr_t suffix_index = 0;
  while(bytes >= (1ull << 11)) {
    bytes >>= 10;
    suffix_index += 1;
  }

  return String(ntos(bytes)) + suffixes[suffix_index];
}