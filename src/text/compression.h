#pragma once

#include "../container/Array.h"
#include "../container/Buffer.h"
#include "../stream/Stream.h"
#include "../text/Symbol.h"

namespace L {
  typedef void CompressFunc(const void* data, size_t size, Stream& out_stream);
  typedef Buffer DecompressFunc(const void* data, size_t size);
  struct Compression {
    Symbol name;
    CompressFunc* compress;
    DecompressFunc* decompress;
  };
  void register_compression(const Compression&);
  const Compression& get_compression(const Symbol& name = "");
  const Array<Compression>& get_compressions();
}
