#pragma once

#include "../container/Buffer.h"
#include "../stream/Stream.h"

namespace L {
  void lz_compress(const void* in_data, size_t in_size, Stream& out_stream);
  Buffer lz_decompress(const void* in_data, size_t in_size);
}
