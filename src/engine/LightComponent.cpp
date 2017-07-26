#include "LightComponent.h"

#include "SharedUniform.h"

using namespace L;

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

void LightComponent::lateUpdate() {
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
  GL::Program& light_program(program());
  light_program.uniform("l_pos", _position);
  light_program.uniform("l_dir", _direction);
  light_program.uniform("l_color", _color);
  light_program.uniform("l_int", _intensity);
  light_program.uniform("l_rad", _radius);
  light_program.uniform("l_in_ang", _inner_angle);
  light_program.uniform("l_out_ang", _outer_angle);
  light_program.uniform("l_type", _type);
  switch(_type) {
    default:
      GL::quad().draw();
      break;
  }
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
      "uniform vec3 l_pos;"
      "uniform vec3 l_dir;"
      "uniform vec3 l_color;"
      "uniform float l_int;"
      "uniform float l_rad;"
      "uniform float l_in_ang;"
      "uniform float l_out_ang;"
      "uniform int l_type;"
      "void main(){"
      "vec3 color = texture(color_buffer,ftexcoords).rgb;"
      "vec3 normal = decodeNormal(texture(normal_buffer,ftexcoords).xy);"
      "float depth = texture(depth_buffer,ftexcoords).r;"
      "vec4 position_p = invViewProj * vec4(ftexcoords*2.f-1.f,depth*2.f-1.f,1.f);"
      "vec3 position = position_p.xyz/position_p.w;"
      "vec3 frag_to_light = (l_pos-position);"
      "vec3 to_light_dir = (l_type==0) ? l_dir : normalize(frag_to_light);"
      "vec3 reflect_dir = reflect(-to_light_dir, normal);"
      "vec3 view_dir = normalize(eye.xyz - position);"
      "float dist = length(frag_to_light);"
      "float att_num = clamp(1.f-pow(dist/l_rad,4.f),0.f,1.f);"
      "float att = l_int*(att_num*att_num)/((dist*dist)+1.f);"
      "if(l_type == 2 && dot(l_dir, -to_light_dir)<l_in_ang) att = 0.f;"
      "if(l_type == 0) att = l_int;"
      "float diff = max(dot(normal,to_light_dir),0.f);"
      "float spec = 0.f; /*pow(max(dot(view_dir, reflect_dir), 0.0), 32.f);*/"
      "fragcolor = vec4(color*l_color*diff*att + l_color*spec*att, 1.f);"
      "}", GL_FRAGMENT_SHADER));
  return program;
}
