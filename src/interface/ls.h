#pragma once

#include <L/src/L.h>

namespace L {
  void ls_loader(Resource<Script::CodeFunction>::Slot& slot) {
    Buffer buffer(slot.read_source_file());
    Script::Compiler compiler;
    compiler.read((const char*)buffer.data(), buffer.size(), true);
    slot.value = ref<Script::CodeFunction>(compiler.function());
  }
}
