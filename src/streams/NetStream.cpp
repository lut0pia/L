#include "NetStream.h"

using namespace L;

#include "../network/Network.h"

size_t NetStream::write(const void* data, size_t size) {
  return ::send(_sd,(const char*)data,size,0);
}
size_t NetStream::read(void* data, size_t size) {
  return ::recv(_sd,(char*)data,size,0);
}
