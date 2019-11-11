#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include "LSCompiler.h"

using namespace L;

static const Symbol ls_symbol("ls");

bool ls_script_loader(ResourceSlot& slot, ScriptFunction& intermediate) {
  if(slot.ext != ls_symbol) {
    return false;
  }

  if(Buffer buffer = slot.read_source_file()) {
    LSCompiler compiler;
    if(compiler.read(slot.id, (const char*)buffer.data(), buffer.size())) {
      if(compiler.compile(intermediate)) {
        return true;
      }
    }
  }
  return false;
}
void ls_module_init() {
  ResourceLoading<ScriptFunction>::add_loader(ls_script_loader);
}
