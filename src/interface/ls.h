#pragma once

#include <L/src/L.h>

namespace L {
  class LS : public Interface<Script::CodeFunction> {
    static LS instance;
  public:
    LS() : Interface{"ls"} {}
    Ref<Script::CodeFunction> from(Stream& stream) override {
      return ref<Script::CodeFunction>(Script::Context::read(stream));
    }
  };
  LS LS::instance;
}
