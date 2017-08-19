#include "NetStream.h"

using namespace L;

#include "../network/Network.h"

size_t NetStream::write(const void* data, size_t size) {
  return Network::send(_sd, (const char*)data, size);
}
size_t NetStream::read(void* data, size_t size) {
  return ::recv(_sd, (char*)data, int(size), 0);
}
