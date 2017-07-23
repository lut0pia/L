#pragma once

#include "Stream.h"

namespace L {
  class NetStream : public Stream {
    protected:
      int _sd;
    public:
      NetStream(int sd) : _sd(sd) {}
      size_t write(const void* data, size_t size);
      size_t read(void* data, size_t size);
  };
}
