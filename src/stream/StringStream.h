#pragma once

#include "Stream.h"
#include "../text/String.h"

namespace L {
  class StringStream : public Stream {
  protected:
    String _string;
  public:
    inline StringStream() {}
    inline size_t write(const void* data, size_t size) override {
      const uintptr_t prev_size(_string.size());
      _string.size(_string.size()+size);
      memcpy(_string+prev_size, data, size);
      return size;
    }
    inline size_t read(void* data, size_t) override { return 0; }
    const String& string() const { return _string; }
  };
  template <class T>
  String to_string(const T& v) {
    StringStream ss;
    ss << v;
    return ss.string();
  }
}
