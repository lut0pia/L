#pragma once

#include <L/src/L.h>

namespace L {
  class LS : public Interface<Script::CodeFunction> {
    static LS instance;
  public:
    LS() : Interface{"ls"} {}
    Ref<Script::CodeFunction> from(const byte* data, size_t size) override {
      Script::Compiler compiler;
      compiler.read((const char*)data, size, true);
      return ref<Script::CodeFunction>(compiler.function());
    }
  };
  LS LS::instance;
}
