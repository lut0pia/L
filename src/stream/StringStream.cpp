#include "StringStream.h"

using namespace L;

size_t StringStream::write(const void* data, size_t size) {
  const uintptr_t prev_size = _string.size();
  _string.size(_string.size() + size);
  memcpy(_string + prev_size, data, size);
  return size;
}
size_t StringStream::read(void* data, size_t size) {
  size = min(size, _string.size() - _index);
  if(size) {
    memcpy(data, _string.begin() + _index, size);
    _index += size;
  }
  _index += (_index == _string.size()); // Indicate end of stream
  return size;
}
