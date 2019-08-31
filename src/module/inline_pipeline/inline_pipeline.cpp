#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Pipeline.h>
#include <L/src/stream/CFileStream.h>

using namespace L;

static const Symbol front_symbol("front"), back_symbol("back"), none_symbol("none"),
mult_symbol("mult"),
light_symbol("light"), present_symbol("present");

bool inline_pip_loader(ResourceSlot& slot, Pipeline::Intermediate& intermediate) {
  if(Symbol vertex_path = slot.parameter("vertex")) {
    intermediate.shaders.push(vertex_path);
  }
  if(Symbol fragment_path = slot.parameter("fragment")) {
    intermediate.shaders.push(fragment_path);
  }
  for(Resource<Shader>& shader : intermediate.shaders) {
    slot.dependencies.push(const_cast<ResourceSlot*>(shader.slot()));
  }

  if(Symbol cull_mode_name = slot.parameter("cull")) {
    if(cull_mode_name==front_symbol) {
      intermediate.cull_mode = VK_CULL_MODE_FRONT_BIT;
    } else if(cull_mode_name==back_symbol) {
      intermediate.cull_mode = VK_CULL_MODE_BACK_BIT;
    } else if(cull_mode_name==none_symbol) {
      intermediate.cull_mode = VK_CULL_MODE_NONE;
    }
  }

  if(Symbol blend_override_name = slot.parameter("blend")) {
    if(blend_override_name==mult_symbol) {
      intermediate.blend_override = Pipeline::BlendOverride::Mult;
    }
  }

  if(Symbol pass_name = slot.parameter("pass")) {
    if(pass_name==light_symbol) {
      intermediate.render_pass = light_symbol;
    } else if(pass_name==present_symbol) {
      intermediate.render_pass = present_symbol;
    }
  }

  return true;
}

void inline_pipeline_module_init() {
  ResourceLoading<Pipeline>::add_loader("inline", inline_pip_loader);
}
