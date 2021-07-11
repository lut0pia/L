#include "compression.h"

using namespace L;

static const Symbol empty_symbol = "", none_symbol = "none";
static Array<Compression> compressions{
  Compression{
    "none",
    [](const void* data, size_t size, Stream& out_stream) { out_stream.write(data, size); },
    [](const void* data, size_t size) { return Buffer(data, size); },
  }};

void L::register_compression(const Compression& compression) {
  compressions.push(compression);
}
const Compression& L::get_compression(const Symbol& name) {
  for(const Compression& compression : compressions) {
    if(compression.name == name || (L_USE_COMPRESSION && name == empty_symbol && compression.name != none_symbol)) {
      return compression;
    }
  }
  return get_compression("none");
}
const Array<Compression>& L::get_compressions() {
  return compressions;
}
