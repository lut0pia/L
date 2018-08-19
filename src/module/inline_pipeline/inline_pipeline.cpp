#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Pipeline.h>
#include <L/src/stream/CFileStream.h>

using namespace L;

static const Symbol front_symbol("front"), back_symbol("back"), none_symbol("none"),
  mult_symbol("mult"),
  light_symbol("light"), present_symbol("present");

bool inline_pip_loader(ResourceSlot& slot, Pipeline*& intermediate) {
  const RenderPass* render_pass(&RenderPass::geometry_pass());
  VkCullModeFlagBits cull_mode(VK_CULL_MODE_BACK_BIT);
  Pipeline::BlendOverride blend_override(Pipeline::BlendOverride::None);
  Array<Resource<Shader>> shaders;
  if(Symbol vertex_path = slot.parameter("vertex")) {
    shaders.push(vertex_path);
  }
  if(Symbol fragment_path = slot.parameter("fragment")) {
    shaders.push(fragment_path);
  }
  for(const Resource<Shader>& shader : shaders) {
    shader.load();
    slot.dependencies.push(shader.slot());
  }

  if(Symbol cull_mode_name = slot.parameter("cull")) {
    if(cull_mode_name==front_symbol) {
      cull_mode = VK_CULL_MODE_FRONT_BIT;
    } else if(cull_mode_name==back_symbol) {
      cull_mode = VK_CULL_MODE_BACK_BIT;
    } else if(cull_mode_name==none_symbol) {
      cull_mode = VK_CULL_MODE_NONE;
    }
  }

  if(Symbol blend_override_name = slot.parameter("blend")) {
    if(blend_override_name==mult_symbol) {
      blend_override = Pipeline::BlendOverride::Mult;
    }
  }

  if(Symbol pass_name = slot.parameter("pass")) {
    if(pass_name==light_symbol) {
      render_pass = &RenderPass::light_pass();
    } else if(pass_name==present_symbol) {
      render_pass = &RenderPass::present_pass();
    }
  }

  const Shader* raw_shaders[8];
  uintptr_t raw_shader_count(0);
  for(const Resource<Shader>& shader : shaders) {
    shader.flush();
    if(!shader) {
      err << "Shader " << slot.id << " couldn't be loaded\n";
      return false;
    }
    raw_shaders[raw_shader_count++] = &*shader;
  }

  intermediate = Memory::new_type<Pipeline>(raw_shaders, raw_shader_count, cull_mode, blend_override, *render_pass);
  return true;
}

void inline_pipeline_module_init() {
  ResourceLoading<Pipeline>::add_loader("inline", inline_pip_loader);
}
