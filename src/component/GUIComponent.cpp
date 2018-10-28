#include "GUIComponent.h"

#include "Camera.h"
#include "../engine/Resource.inl"

using namespace L;

void GUIComponent::update_components() {
  _camera = entity()->component<Camera>();
}
Map<Symbol, Var> GUIComponent::pack() const {
  return {
    {"material", _material},
    {"offset", _offset},
    {"viewport_anchor", _viewport_anchor},
    {"anchor", _anchor},
    {"scale", _scale},
  };
}
void GUIComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "material", _material);
  unpack_item(data, "scale", _scale);
}
void GUIComponent::script_registration() {
  L_COMPONENT_BIND(GUIComponent, "gui");
  L_COMPONENT_RETURN_METHOD(GUIComponent, "material", 0, material());
  L_COMPONENT_METHOD(GUIComponent, "offset", 2, offset(Vector2i(c.param(0).get<int>(), c.param(1).get<int>())));
  L_COMPONENT_METHOD(GUIComponent, "viewport-anchor", 2, viewport_anchor(Vector2f(c.param(0).get<float>(), c.param(1).get<float>())));
  L_COMPONENT_METHOD(GUIComponent, "anchor", 2, anchor(Vector2f(c.param(0).get<float>(), c.param(1).get<float>())));
  L_COMPONENT_METHOD(GUIComponent, "scale", 2, scale(Vector2f(c.param(0).get<float>(), c.param(1).get<float>())));
}

void GUIComponent::gui(const Camera& camera) {
  _material.update();
  if(&camera==_camera && _material.valid_for_render_pass(RenderPass::present_pass())) {
    const Vector2f dimensions(_material.gui_size()*_scale);
    const Matrix44f model(translation_matrix(Vector3f(_viewport_anchor.x()*2.f-1.f, _viewport_anchor.y()*2.f-1.f, 0))
      * scale_matrix(Vector3f(dimensions.x()/camera.geometry_buffer().width(), dimensions.y()/camera.geometry_buffer().height(), 0.f))
      * translation_matrix(Vector3f(-(_anchor.x()*2.f-1.f)+float(_offset.x()*2.f)/dimensions.x(), -(_anchor.y()*2.f-1.f)+float(_offset.y()*2.f)/dimensions.y(), 0)));
    _material.draw(camera, RenderPass::present_pass(), model);
  }
}
