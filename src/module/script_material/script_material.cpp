#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Material.h>
#include <L/src/script/ScriptContext.h>

using namespace L;

bool script_material_loader(ResourceSlot& slot, Material& intermediate) {
  ScriptFunction::Intermediate script_intermediate;
  if(ResourceLoading<ScriptFunction>::load_internal(slot, script_intermediate)) {
    ScriptContext context(&intermediate);
    context.execute(ref<ScriptFunction>(script_intermediate));
    return true;
  }
  return false;
}

void script_material_module_init() {
  ResourceLoading<Material>::add_loader(script_material_loader);
}
