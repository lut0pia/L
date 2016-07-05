#include "StaticMesh.h"

#include "../gl/Program.h"

using namespace L;

void StaticMesh::render(const Camera& c){
  static GL::Program program(GL::Shader(
    "#version 330 core\n"
    "layout (location = 0) in vec3 vposition;"
    "layout (location = 1) in vec2 vtexcoords;"
    "layout (location = 2) in vec3 vnormal;"
    "layout (std140) uniform Shared {mat4 viewProj;};"
    "uniform mat4 model;"
    "out vec2 ftexcoords;"
    "out vec3 fnormal;"
    "void main(){"
    "ftexcoords = vtexcoords;"
    "fnormal = vnormal;"
    "gl_Position = viewProj * model * vec4(vposition,1.0);"
    "}",GL_VERTEX_SHADER),
    GL::Shader(
      "#version 330 core\n"
      "layout(location = 0) out vec3 ocolor;"
      "layout(location = 1) out vec3 onormal;"
      "uniform sampler2D tex;"
      "in vec2 ftexcoords;"
      "in vec3 fnormal;"
      "void main(){"
      //"ocolor = vec3(ftexcoords,0.f);"
      "ocolor = texture(tex,ftexcoords).rgb;"
      "onormal = fnormal;"
      "}",GL_FRAGMENT_SHADER));
  program.use();
  program.uniform("tex",*_texture);
  program.uniform("model",_transform->matrix());
  _mesh->draw();
}