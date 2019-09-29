#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Material.h>
#include <L/src/script/ScriptContext.h>
#include <L/src/engine/Engine.h>
#include <L/src/engine/Settings.h>
#include "LSCompiler.h"

using namespace L;

static const Symbol ls_symbol("ls");

bool ls_script_loader(ResourceSlot& slot, ScriptFunction& intermediate) {
  if(slot.ext != ls_symbol) {
    return false;
  }

  if(Buffer buffer = slot.read_source_file()) {
    intermediate.offset = 0;
    intermediate.script = ref<Script>();
    LSCompiler compiler;
    if(compiler.read((const char*)buffer.data(), buffer.size(), true)) {
      intermediate = compiler.compile();
      return true;
    }
  }
  return false;
}
void ls_module_init() {
  ResourceLoading<ScriptFunction>::add_loader(ls_script_loader);
}
