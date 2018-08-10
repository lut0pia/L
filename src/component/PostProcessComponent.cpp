#include "PostProcessComponent.h"

using namespace L;

Map<Symbol, Var> PostProcessComponent::pack() const {
  return Map<Symbol, Var> {
    { "material", _material },
  };
}
void PostProcessComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "material", _material);
}
void PostProcessComponent::script_registration() {
  L_COMPONENT_BIND(PostProcessComponent, "post-process");
  L_COMPONENT_RETURN_METHOD(PostProcessComponent, "material", 0, material());
}

void PostProcessComponent::render(const Camera& camera, const RenderPass& render_pass) {
  if(_material.valid() && &_material.final_pipeline()->render_pass()==&render_pass) {
    _material.draw(camera, render_pass);
  }
}
