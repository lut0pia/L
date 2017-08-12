#include "LightComponent.h"

#include "../gl/GL.h"
#include "../engine/SharedUniform.h"

using namespace L;

Resource<GL::Program> LightComponent::_program;

Map<Symbol, Var> LightComponent::pack() const {
  Map<Symbol, Var> data;
  data["color"] = _color;
  data["direction"] = _direction;
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
  unpack_item(data, "direction", _direction);
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
  Script::Context::global(Symbol("light-program")) = (Script::Function)([](Script::Context& c) {
    if(c.localCount()>0)
      LightComponent::program(c.local(0).get<String>());
  });
}

void LightComponent::late_update() {
  _position = _transform->position();
  if(_type == 2)
    _direction = _transform->rotation().rotate(_relative_dir);
}
void LightComponent::directional(const Color& color, const Vector3f& direction, float intensity) {
  _type = 0;
  _color = Color::to_float_vector(color);
  _direction = direction.normalized();
  _intensity = intensity;
  _radius = 0.f;
}
void LightComponent::point(const Color& color, float intensity, float radius) {
  _type = 1;
  _color = Color::to_float_vector(color);
  _intensity = intensity;
  _radius = radius;
}
void LightComponent::spot(const Color& color, const Vector3f& relative_dir, float intensity, float radius, float inner_angle, float outer_angle) {
  _type = 2;
  _relative_dir = relative_dir.normalized();
  _color = Color::to_float_vector(color);
  _intensity = intensity;
  _radius = radius;
  _inner_angle = inner_angle;
  _outer_angle = outer_angle>0.f ? outer_angle : inner_angle;
}

void LightComponent::render() {
  _program->uniform("l_pos", _position);
  _program->uniform("l_dir", _direction);
  _program->uniform("l_color", _color);
  _program->uniform("l_int", _intensity);
  _program->uniform("l_rad", _radius);
  _program->uniform("l_in_ang", _inner_angle);
  _program->uniform("l_out_ang", _outer_angle);
  _program->uniform("l_type", _type);
  GL::draw(GL_TRIANGLES, 3); // TODO: Draw spatialized shape from light type and position
}
