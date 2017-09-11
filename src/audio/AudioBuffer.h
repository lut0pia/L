#pragma once

#include "../container/Raw.h"
#include "../types.h"

namespace L {
  namespace Audio {
    class Buffer {
    protected:
      Raw<8> _data;
    public:
      enum Format {
        Mono8, Mono16,
        Stereo8, Stereo16,
      };
      Buffer();
      void data(Format, const void* data, size_t size, size_t frequency);
      uint32_t bid() const;
    };
  }
}