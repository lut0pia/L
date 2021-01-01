#include "GUIComponent.h"

#include "Camera.h"
#include "../engine/Resource.inl"

using namespace L;

void GUIComponent::update_components() {
  _camera = entity()->get_component<Camera>();
}
void GUIComponent::script_registration() {
  L_COMPONENT_BIND(GUIComponent, "gui");
  L_SCRIPT_RETURN_METHOD(GUIComponent, "material", 0, material().handle());
  L_SCRIPT_METHOD(GUIComponent, "offset", 2, offset(Vector2i(c.param(0).get<int>(), c.param(1).get<int>())));
  L_SCRIPT_METHOD(GUIComponent, "viewport_anchor", 2, viewport_anchor(Vector2f(c.param(0).get<float>(), c.param(1).get<float>())));
  L_SCRIPT_METHOD(GUIComponent, "anchor", 2, anchor(Vector2f(c.param(0).get<float>(), c.param(1).get<float>())));
  L_SCRIPT_METHOD(GUIComponent, "scale", 2, scale(Vector2f(c.param(0).get<float>(), c.param(1).get<float>())));
}

void GUIComponent::gui(const Camera& camera) {
  _material.update();
  if(&camera == _camera && _material.valid_for_render_pass(RenderPass::present_pass())) {
    const Vector2f dimensions = _material.gui_size() * _scale;
    const Interval2i viewport_pixel = camera.viewport_pixel();
    const Vector2i viewport_pixel_size = viewport_pixel.size();
    Matrix44f model = translation_matrix(Vector3f(_viewport_anchor.x() * 2.f - 1.f, _viewport_anchor.y() * 2.f - 1.f, 0.f))
      * scale_matrix(Vector3f(dimensions.x() / viewport_pixel_size.x(), dimensions.y() / viewport_pixel_size.y(), 0.f))
      * translation_matrix(Vector3f(-(_anchor.x() * 2.f - 1.f) + float(_offset.x() * 2.f) / dimensions.x(), -(_anchor.y() * 2.f - 1.f) + float(_offset.y() * 2.f) / dimensions.y(), 0.f));

    if(_material.is_text()) {
      // This is weird but in case we are drawing text and not a texture, the coordinates of the text are not normalized
      model = model
        * translation_matrix(Vector3f(-1.f, -1.f, 0.f))
        * scale_matrix(Vector3f(2.f * _scale.x() / dimensions.x(), 2.f * _scale.y() / dimensions.y(), 0.f));
    }

    _material.draw(camera, RenderPass::present_pass(), model);
  }
}
