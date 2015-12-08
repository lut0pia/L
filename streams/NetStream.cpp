#include "NetStream.h"

using namespace L;

#include "../network/Network.h"

void NetStream::flush() {
  _writeBuffer.rewind();
  size_t bufsize;
  const char* buffer;
  while(buffer = _writeBuffer.bufferize(&bufsize))
    ::send(_sd,buffer,bufsize,0);
  _writeBuffer.rewind();
}
