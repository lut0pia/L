#include "StaticMesh.h"

#include "../gl/GL.h"
#include "../gl/Program.h"
#include "SharedUniform.h"

using namespace L;

Map<Symbol, Var> StaticMesh::pack() const {
  Map<Symbol, Var> data;
  data["mesh"] = _mesh;
  data["texture"] = _texture;
  data["scale"] = _scale;
  return data;
}
void StaticMesh::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "mesh", _mesh);
  unpack_item(data, "texture", _texture);
  unpack_item(data, "scale", _scale);
}

void StaticMesh::render(const Camera& c) {
  static GL::Program program(GL::Shader(
    "#version 330 core\n"
    L_SHAREDUNIFORM
    "layout (location = 0) in vec3 vposition;"
    "layout (location = 1) in vec2 vtexcoords;"
    "layout (location = 2) in vec3 vnormal;"
    "uniform mat4 model;"
    "out vec2 ftexcoords;"
    "out vec3 fnormal;"
    "void main(){"
    "ftexcoords = vtexcoords;"
    "fnormal = normalize(mat3(model) * vnormal);"
    "gl_Position = viewProj * model * vec4(vposition,1.0);"
    "}", GL_VERTEX_SHADER),
    GL::Shader(
      "#version 330 core\n"
      L_SHAREDUNIFORM
      L_SHADER_LIB
      "layout(location = 0) out vec3 ocolor;"
      "layout(location = 1) out vec3 onormal;"
      "uniform sampler2D tex;"
      "in vec2 ftexcoords;"
      "in vec3 fnormal;"
      "void main(){"
      "vec4 color = texture(tex,ftexcoords);"
      "if(alpha(color.a)) discard;"
      "ocolor = color.rgb;"
      "onormal.xy = encodeNormal(fnormal);"
      "}", GL_FRAGMENT_SHADER));
  if(_mesh) {
    program.use();
    program.uniform("tex", _texture ? *_texture : GL::whiteTexture());
    program.uniform("model", SQTToMat(_transform->rotation(), _transform->position(), _scale));
    _mesh->draw();
  }
}
