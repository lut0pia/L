#pragma once

#include "Stream.h"

namespace L {
  class FileStream : public Stream {
  public:
    virtual void rewind() { seek(0); }
    virtual void seek(int) = 0;
    virtual int tell() const = 0;
    virtual size_t size() = 0;
  };
}
