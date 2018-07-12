#include "DirectStream.h"

#include "../container/Buffer.h"

using namespace L;

Buffer DirectStream::read_into_buffer() {
  Buffer buffer(size());
  seek(0);
  read(buffer, buffer.size());
  return buffer;
}
