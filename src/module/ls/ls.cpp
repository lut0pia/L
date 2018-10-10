#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Material.h>
#include <L/src/script/ScriptContext.h>
#include <L/src/engine/Engine.h>
#include <L/src/engine/Settings.h>
#include "LSCompiler.h"
#include "LSServer.h"

using namespace L;

bool ls_material_loader(ResourceSlot& slot, Material*& intermediate) {
  if(Buffer buffer = slot.read_source_file()) {
    ScriptFunction script_function;
    script_function.offset = 0;
    script_function.script = ref<Script>();
    LSCompiler compiler;
    if(compiler.read((const char*)buffer.data(), buffer.size(), true)) {
      intermediate = Memory::new_type<Material>();
      ScriptContext context(intermediate);
      context.execute(compiler.compile());
      return true;
    }
  }
  return false;
}

bool ls_script_loader(ResourceSlot& slot, ScriptFunction& intermediate) {
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
  ResourceLoading<ScriptFunction>::add_loader("ls", ls_script_loader);
  ResourceLoading<Material>::add_loader("ls", ls_material_loader);

#ifdef L_DEBUG
  static LSServer* server(nullptr);
  if(Settings::get_int("script_server", 0)) {
    server = Memory::new_type<LSServer>(short(Settings::get_int("server_port", 1993)));
    Engine::add_update([]() {
      server->update();
    });
  }
#endif
}
