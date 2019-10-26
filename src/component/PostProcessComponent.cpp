#include "PostProcessComponent.h"

#include "../engine/Resource.inl"

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
  L_COMPONENT_BIND(PostProcessComponent, "post_process");
  L_SCRIPT_RETURN_METHOD(PostProcessComponent, "material", 0, material());
}

void PostProcessComponent::render(const Camera& camera, const RenderPass& render_pass) {
  _material.update();
  if(_material.valid_for_render_pass(render_pass)) {
    _material.draw(camera, render_pass);
  }
}
