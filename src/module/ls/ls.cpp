#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Material.h>
#include <L/src/script/Compiler.h>
#include <L/src/script/Context.h>

using namespace L;

bool ls_material_loader(ResourceSlot& slot, Material*& intermediate) {
  if(Buffer buffer = slot.read_source_file()) {
    Script::Compiler compiler;
    compiler.read((const char*)buffer.data(), buffer.size(), true);
    if(compiler.ready()) {
      intermediate = Memory::new_type<Material>();
      Script::Context context(intermediate);
      context.executeInside(Array<Var>{ref<Script::CodeFunction>(compiler.function())});
      return true;
    }
  }
  return false;
}

bool ls_script_loader(ResourceSlot& slot, Script::CodeFunction*& intermediate) {
  if(Buffer buffer = slot.read_source_file()) {
    Script::Compiler compiler;
    compiler.read((const char*)buffer.data(), buffer.size(), true);
    if(compiler.ready()) {
      intermediate = Memory::new_type<Script::CodeFunction>(compiler.function());
      return true;
    }
  }
  return false;
}

void ls_module_init() {
  ResourceLoading<Script::CodeFunction>::add_loader("ls", ls_script_loader);
  ResourceLoading<Material>::add_loader("ls", ls_material_loader);
}
