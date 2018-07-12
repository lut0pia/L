#include "LightComponent.h"

#include "../engine/Engine.h"
#include "../rendering/shader_lib.h"

using namespace L;

Resource<Pipeline> LightComponent::_pipeline;

Map<Symbol, Var> LightComponent::pack() const {
  Map<Symbol, Var> data;
  data["color"] = _color;
  data["relative_dir"] = _relative_dir;
  data["intensity"] = _intensity;
  data["radius"] = _radius;
  data["inner_angle"] = _inner_angle;
  data["outer_angle"] = _outer_angle;
  data["type"] = _type;
  return data;
}
void LightComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "color", _color);
  unpack_item(data, "relative_dir", _relative_dir);
  unpack_item(data, "intensity", _intensity);
  unpack_item(data, "radius", _radius);
  unpack_item(data, "inner_angle", _inner_angle);
  unpack_item(data, "outer_angle", _outer_angle);
  unpack_item(data, "type", _type);
}
void LightComponent::script_registration() {
  L_COMPONENT_BIND(LightComponent, "light");
  L_COMPONENT_METHOD(LightComponent, "directional", 3, directional(c.local(0).get<Color>(), c.local(1).get<Vector3f>(), c.local(2).get<float>()));
  L_COMPONENT_METHOD(LightComponent, "point", 3, point(c.local(0).get<Color>(), c.local(1).get<float>(), c.local(2).get<float>()));
  L_COMPONENT_METHOD(LightComponent, "spot", 5, spot(c.local(0).get<Color>(), c.local(1).get<Vector3f>(), c.local(2).get<float>(), c.local(3).get<float>(), c.local(4).get<float>()));
  Script::Context::global(Symbol("light-pipeline")) = (Script::Function)([](Script::Context& c) {
    if(c.localCount()>0)
      LightComponent::pipeline(c.local(0).get<String>());
  });
}

void LightComponent::late_update() {
  _cull_volume.update_bounds(Interval3f(_transform->position()).extended(_radius));
}
void LightComponent::directional(const Color& color, const Vector3f& direction, float intensity) {
  _type = 0;
  _color = Color::to_float_vector(color);
  _intensity = intensity;
  _radius = 0.f;

  _desc_set.set_value("l_dir", direction.normalized());
  _desc_set.set_value("l_color", _color);
  _desc_set.set_value("l_int", _intensity);
  _desc_set.set_value("l_rad", _radius);
  _desc_set.set_value("l_in_ang", _inner_angle);
  _desc_set.set_value("l_out_ang", _outer_angle);
  _desc_set.set_value("l_type", _type);
}
void LightComponent::point(const Color& color, float intensity, float radius) {
  _type = 1;
  _color = Color::to_float_vector(color);
  _intensity = intensity;
  _radius = radius;

  _desc_set.set_value("l_color", _color);
  _desc_set.set_value("l_int", _intensity);
  _desc_set.set_value("l_rad", _radius);
  _desc_set.set_value("l_in_ang", _inner_angle);
  _desc_set.set_value("l_out_ang", _outer_angle);
  _desc_set.set_value("l_type", _type);
}
void LightComponent::spot(const Color& color, const Vector3f& relative_dir, float intensity, float radius, float inner_angle, float outer_angle) {
  _type = 2;
  _relative_dir = relative_dir.normalized();
  _color = Color::to_float_vector(color);
  _intensity = intensity;
  _radius = radius;
  _inner_angle = inner_angle;
  _outer_angle = outer_angle>0.f ? outer_angle : inner_angle;

  _desc_set.set_value("l_dir", _relative_dir);
  _desc_set.set_value("l_color", _color);
  _desc_set.set_value("l_int", _intensity);
  _desc_set.set_value("l_rad", _radius);
  _desc_set.set_value("l_in_ang", _inner_angle);
  _desc_set.set_value("l_out_ang", _outer_angle);
  _desc_set.set_value("l_type", _type);
}

void LightComponent::render(VkCommandBuffer cmd_buffer, const Framebuffer& framebuffer) {
  if(_type == 0 || _cull_volume.visible()) {
    _desc_set.set_descriptor("color_buffer", VkDescriptorImageInfo {Vulkan::sampler(), framebuffer.image_view(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    _desc_set.set_descriptor("normal_buffer", VkDescriptorImageInfo {Vulkan::sampler(), framebuffer.image_view(1), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    _desc_set.set_descriptor("depth_buffer", VkDescriptorImageInfo {Vulkan::sampler(), framebuffer.image_view(2), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});

    const Matrix44f model(_transform->matrix());
    vkCmdPushConstants(cmd_buffer, *_pipeline, _pipeline->find_binding("Constants")->stage, 0, sizeof(model), &model);
    vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *_pipeline, 0, 1, &(const VkDescriptorSet&)_desc_set, 0, nullptr);
    uint32_t vertex_count(_type==1?(8*3*4):3);
    vkCmdDraw(cmd_buffer, vertex_count, 1, 0, 0);
  }
}
