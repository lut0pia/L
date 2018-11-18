#pragma once

#include "Stream.h"
#include "../text/String.h"

namespace L {
  class StringStream : public Stream {
  protected:
    String _string;
    uintptr_t _index = 0;
  public:
    size_t write(const void*, size_t) override;
    size_t read(void*, size_t) override;
    const String& string() const { return _string; }
  };
  template <class T>
  String to_string(const T& v) {
    StringStream ss;
    ss << v;
    return ss.string();
  }
}
