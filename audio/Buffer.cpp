#include "Buffer.h"

#include "../Exception.h"

using namespace L;
using namespace Audio;

Buffer::Buffer(const Format& format, size_t bytes) : format(format), bytes(bytes) {}
Buffer::Buffer(const Format& format, const Time& time) : format(format), bytes(time.seconds()*format.bytesPerSecond()) {}
Buffer::Buffer(const Format& format, const Buffer& buffer) : format(format) {
  if(format!=buffer.format)
    throw Exception("Conversion of audio format is not implemented.");
  else bytes = buffer.bytes;
}
