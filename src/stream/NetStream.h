#pragma once

#include "Stream.h"
#include "../network/Network.h"

namespace L {
  class NetStream : public Stream {
    protected:
      SOCKET _sd;
    public:
      NetStream(SOCKET sd) : _sd(sd) {}
      size_t write(const void* data, size_t size);
      size_t read(void* data, size_t size);
  };
}
