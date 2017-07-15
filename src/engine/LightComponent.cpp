#include "LightComponent.h"

#include "SharedUniform.h"

using namespace L;

Map<Symbol, Var> LightComponent::pack() const {
  Map<Symbol, Var> data;
  data["color"] = _color;
  data["direction"] = _direction;
  data["relative_dir"] = _relative_dir;
  data["linear_attenuation"] = _linear_attenuation;
  data["quadratic_attenuation"] = _quadratic_attenuation;
  data["inner_angle"] = _inner_angle;
  data["outer_angle"] = _outer_angle;
  data["type"] = _type;
  return data;
}
void LightComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "color", _color);
  unpack_item(data, "direction", _direction);
  unpack_item(data, "relative_dir", _relative_dir);
  unpack_item(data, "linear_attenuation", _linear_attenuation);
  unpack_item(data, "quadratic_attenuation", _quadratic_attenuation);
  unpack_item(data, "inner_angle", _inner_angle);
  unpack_item(data, "outer_angle", _outer_angle);
  unpack_item(data, "type", _type);
}

void LightComponent::lateUpdate() {
  _position = _transform->position();
  if(_type == 2)
    _direction = _transform->rotation().rotate(_relative_dir);
}
void LightComponent::directional(const Color& color, const Vector3f& direction) {
  _type = 0;
  _color = Color::to_float_vector(color);
  _direction = direction.normalized();
  _linear_attenuation = _quadratic_attenuation = 0.f;
}
void LightComponent::point(const Color& color, float linear_attenuation, float quadratic_attenuation) {
  _type = 1;
  _color = Color::to_float_vector(color);
  _linear_attenuation = linear_attenuation;
  _quadratic_attenuation = quadratic_attenuation;
}
void LightComponent::spot(const Color& color, const Vector3f& relative_dir, float linear_attenuation, float quadratic_attenuation, float inner_angle, float outer_angle) {
  _type = 2;
  _relative_dir = relative_dir.normalized();
  _color = Color::to_float_vector(color);
  _linear_attenuation = linear_attenuation;
  _quadratic_attenuation = quadratic_attenuation;
  _inner_angle = inner_angle;
  _outer_angle = outer_angle>0.f ? outer_angle : inner_angle;
}

void LightComponent::render() {
  GL::Program& light_program(program());
  light_program.uniform("light_pos", _position);
  light_program.uniform("light_dir", _direction);
  light_program.uniform("light_color", _color);
  light_program.uniform("light_lin", _linear_attenuation);
  light_program.uniform("light_quad", _quadratic_attenuation);
  light_program.uniform("light_in_ang", _inner_angle);
  light_program.uniform("light_out_ang", _outer_angle);
  light_program.uniform("light_type", _type);
  GL::quad().draw();
}

GL::Program& LightComponent::program() {
  static GL::Program program(GL::Shader(
    "#version 330 core\n"
    "layout (location = 0) in vec3 vertex;"
    "out vec2 ftexcoords;"
    "void main(){"
    "ftexcoords = (vertex.xy+1.f)*.5f;"
    "gl_Position = vec4(vertex,1.f);"
    "}", GL_VERTEX_SHADER),
    GL::Shader(
      "#version 330 core\n"
      L_SHAREDUNIFORM
      L_SHADER_LIB
      "in vec2 ftexcoords;"
      "out vec4 fragcolor;"
      "uniform sampler2D color_buffer;"
      "uniform sampler2D normal_buffer;"
      "uniform sampler2D depth_buffer;"
      "uniform vec3 light_pos;"
      "uniform vec3 light_dir;"
      "uniform vec3 light_color;"
      "uniform float light_lin;"
      "uniform float light_quad;"
      "uniform float light_in_ang;"
      "uniform float light_out_ang;"
      "uniform int light_type;"
      "void main(){"
      "vec3 color = texture(color_buffer,ftexcoords).rgb;"
      "vec3 normal = decodeNormal(texture(normal_buffer,ftexcoords).xy);"
      "float depth = texture(depth_buffer,ftexcoords).r;"
      "vec4 position_p = invViewProj * vec4(ftexcoords*2.f-1.f,depth*2.f-1.f,1.f);"
      "vec3 position = position_p.xyz/position_p.w;"
      "vec3 frag_to_light = (light_pos-position);"
      "vec3 to_light_dir = (light_type==0) ? light_dir : normalize(frag_to_light);"
      "vec3 reflect_dir = reflect(-to_light_dir, normal);"
      "vec3 view_dir = normalize(eye.xyz - position);"
      "float dist = length(frag_to_light);"
      "float att = 1.f / (1.f + light_lin * dist + light_quad * dist * dist);"
      "if(light_type == 2 && dot(light_dir, -to_light_dir)<light_in_ang) att = 0.f;"
      "float diff = max(dot(normal,to_light_dir),0.f);"
      "float spec = 0.f; /*pow(max(dot(view_dir, reflect_dir), 0.0), 32.f);*/"
      "fragcolor = vec4(color*light_color*diff*att + light_color*spec*att, 1.f);"
      "}", GL_FRAGMENT_SHADER));
  return program;
}
