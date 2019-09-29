#pragma once

#include "DirectStream.h"
#include "../macros.h"

namespace L {
  class AsyncFileStream : public DirectStream {
  protected:
    void* _data;
  public:
    AsyncFileStream(const char* filepath, const char* mode);
    ~AsyncFileStream();

    size_t write(const void* data, size_t size) override;
    size_t read(void* data, size_t size) override;
    bool end() override;

    uintptr_t tell() override;
    void seek(uintptr_t i) override;
    size_t size() override;

    operator bool() const;
  };
}
