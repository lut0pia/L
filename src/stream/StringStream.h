#pragma once

#include "../text/String.h"
#include "DirectStream.h"

namespace L {
  class StringStream : public DirectStream {
  protected:
    String _string;
    uintptr_t _index = 0;

  public:
    virtual size_t write(const void*, size_t) override;
    virtual size_t read(void*, size_t) override;

    virtual char peek() override { return _string[_index]; }
    virtual void seek(uintptr_t index) override { _index = index; }
    virtual uintptr_t tell() override { return _index; }
    virtual size_t size() override { return _string.size(); }

    const String& string() const { return _string; }
  };
  template <class T>
  String to_string(const T& v) {
    StringStream ss;
    ss << v;
    return ss.string();
  }
}
