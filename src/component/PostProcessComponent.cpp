#include "PostProcessComponent.h"

#include "../engine/Resource.inl"

using namespace L;

void PostProcessComponent::script_registration() {
  L_COMPONENT_BIND(PostProcessComponent, "post_process");
  L_SCRIPT_RETURN_METHOD(PostProcessComponent, "material", 0, material().handle());
}

void PostProcessComponent::render(const Camera& camera, const RenderPassImpl* render_pass) {
  _material.update();
  if(_material.valid_for_render_pass(render_pass)) {
    _material.draw(camera, render_pass);
  }
}
