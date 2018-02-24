#include "Sprite.h"

#include "../rendering/Program.h"
#include "../rendering/Shader.h"
#include "../rendering/shader_lib.h"

using namespace L;

Map<Symbol, Var> Sprite::pack() const {
  Map<Symbol, Var> data;
  data["texture"] = _texture;
  data["vertex"] = _vertex;
  data["uv"] = _uv;
  return data;
}
void Sprite::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "texture", _texture);
  unpack_item(data, "vertex", _vertex);
  unpack_item(data, "uv", _uv);
}
void Sprite::script_registration() {
  L_COMPONENT_BIND(Sprite, "sprite");
  L_COMPONENT_METHOD(Sprite, "texture", 1, texture(c.local(0).get<String>()));
  L_COMPONENT_METHOD(Sprite, "vertex", 4, vertex(Interval2f(Vector2f(c.local(0).get<float>(), c.local(1).get<float>()),
                                                            Vector2f(c.local(2).get<float>(), c.local(3).get<float>()))));
  L_COMPONENT_METHOD(Sprite, "uv", 4, uv(Interval2f(Vector2f(c.local(0).get<float>(), c.local(1).get<float>()),
                                                    Vector2f(c.local(2).get<float>(), c.local(3).get<float>()))));
}

void Sprite::render(const Camera&) {
  static Program program(Shader(
    L_GLSL_INTRO
    L_SHAREDUNIFORM
    "uniform vec4 vert;"
    "uniform vec4 uv;"
    "uniform mat4 model;"
    "out vec2 ftexcoords;"
    "out vec3 fnormal;"
    "const int[] indices = int[](1,0,2,1,2,3);"
    "void main(){"
    "int index = indices[gl_VertexID];"
    "ftexcoords = vec2(uv[index/2],uv[1-index%2+2]);"
    "fnormal = mat3(model)*vec3(0,-1,0);"
    "gl_Position = viewProj * model * vec4(vert[index/2],0.f,vert[index%2+2],1.f);"
    "}", GL_VERTEX_SHADER),
    Shader(
      L_GLSL_INTRO
      L_SHAREDUNIFORM
      L_SHADER_LIB
      "layout(location = 0) out vec4 ocolor;"
      "layout(location = 1) out vec4 onormal;"
      "uniform sampler2D tex;"
      "in vec2 ftexcoords;"
      "in vec3 fnormal;"
      "void main(){"
      "vec4 color = texture(tex,ftexcoords);"
      "if(alpha(color.a)) discard;"
      "ocolor.rgb = linearize(color.rgb);"
      "ocolor.a = 0.f;" /* Metalness */
      "onormal.xy = encodeNormal(fnormal);"
      "onormal.z = 1.f;" /* Roughness */
      "onormal.w = 0.f;" /* Emission */
      "}", GL_FRAGMENT_SHADER));
  if(_texture) {
    program.use();
    program.uniform("tex", *_texture);
    program.uniform("model", _transform->matrix());
    program.uniform("vert", Vector4f(_vertex.min().x(), _vertex.max().x(), _vertex.min().y(), _vertex.max().y()));
    program.uniform("uv", Vector4f(_uv.min().x(), _uv.max().x(), _uv.min().y(), _uv.max().y()));
    GL::draw(GL_TRIANGLES, 6);
  }
}
