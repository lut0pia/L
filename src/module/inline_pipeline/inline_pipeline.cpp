#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Pipeline.h>
#include <L/src/stream/CFileStream.h>

using namespace L;

void inline_pip_loader(ResourceSlot& slot, Pipeline*& intermediate) {
  const RenderPass* render_pass(&RenderPass::geometry_pass());
  Array<Resource<Shader>> shaders;
  if(Symbol vertex_path = slot.parameter("vertex")) {
    shaders.push(vertex_path);
  }
  if(Symbol fragment_path = slot.parameter("fragment")) {
    shaders.push(fragment_path);
  }
  if(Symbol pass_name = slot.parameter("pass")) {
    static const Symbol light_symbol("light"), present_symbol("present");
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
      return;
    }
    raw_shaders[raw_shader_count++] = &*shader;
  }

  intermediate = Memory::new_type<Pipeline>(raw_shaders, raw_shader_count, *render_pass);
}

void inline_pipeline_module_init() {
  ResourceLoading<Pipeline>::add_loader("inline", inline_pip_loader);
}
