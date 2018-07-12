#pragma once

#include "Stream.h"

namespace L {
  class DirectStream : public Stream {
  public:
    virtual size_t peek_read(void* data, size_t size) {
      uintptr_t pos(tell());
      size_t r(read(data, size));
      seek(pos);
      return r;
    }
    virtual char peek() = 0;
    virtual void rewind() { seek(0); }
    virtual void seek(uintptr_t) = 0;
    virtual uintptr_t tell() = 0;
    virtual size_t size() = 0;

    virtual class Buffer read_into_buffer();
  };
}
