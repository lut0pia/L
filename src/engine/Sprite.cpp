#include "Sprite.h"

#include "../gl/Program.h"
#include "../gl/Shader.h"
#include "SharedUniform.h"

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
  update_mesh();
}

void Sprite::vertex(const Interval2f& v) {
  _vertex = v;
  update_mesh();
}
void Sprite::uv(const Interval2f& u) {
  _uv = u;
  update_mesh();
}
void Sprite::update_mesh() {
  const GLfloat quad[] = {
    _vertex.min().x(),_vertex.min().y(),
    _uv.min().x(),_uv.max().y(),
    _vertex.max().x(),_vertex.min().y(),
    _uv.max().x(),_uv.max().y(),
    _vertex.min().x(),_vertex.max().y(),
    _uv.min().x(),_uv.min().y(),
    _vertex.max().x(),_vertex.max().y(),
    _uv.max().x(),_uv.min().y(),
  };
  _mesh.load(GL_TRIANGLE_STRIP, 4, quad, sizeof(quad), {
    {0,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,0},
    {1,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,sizeof(float)*2},
  });
}
void Sprite::render(const Camera&) {
  static GL::Program program(GL::Shader(
    "#version 330 core\n"
    "layout (location = 0) in vec2 vposition;"
    "layout (location = 1) in vec2 vtexcoords;"
    L_SHAREDUNIFORM
    "uniform mat4 model;"
    "out vec2 ftexcoords;"
    "void main(){"
    "ftexcoords = vtexcoords;"
    "gl_Position = viewProj * model * vec4(vposition.x,0.f,vposition.y,1.f);"
    "}", GL_VERTEX_SHADER),
    GL::Shader(
      "#version 330 core\n"
      L_SHAREDUNIFORM
      L_SHADER_LIB
      "layout(location = 0) out vec3 ocolor;"
      "layout(location = 1) out vec3 onormal;"
      "uniform mat4 model;"
      "uniform sampler2D tex;"
      "in vec2 ftexcoords;"
      "void main(){"
      "vec4 color = texture(tex,ftexcoords);"
      "if(alpha(color.a)) discard;"
      "ocolor = color.rgb;"
      "onormal.xy = encodeNormal(mat3(model)*vec3(0,-1,0));"
      "}", GL_FRAGMENT_SHADER));
  if(_texture) {
    program.use();
    program.uniform("tex", *_texture);
    program.uniform("model", _transform->matrix());
    _mesh.draw();
  }
}
