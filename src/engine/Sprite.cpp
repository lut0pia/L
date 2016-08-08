#include "Sprite.h"

#include "../gl/Program.h"
#include "../gl/Shader.h"
#include "SharedUniform.h"

using namespace L;

void Sprite::render(const Camera&) {
  static GL::Program program(GL::Shader(
    "#version 330 core\n"
    "layout (location = 0) in vec3 vposition;"
    L_SHAREDUNIFORM
    "uniform mat4 model;"
    "out vec2 ftexcoords;"
    "void main(){"
    "ftexcoords = (vposition.xy+1.f)*.5f;;"
    "gl_Position = viewProj * model * vec4(vposition,1.0);"
    "}",GL_VERTEX_SHADER),
    GL::Shader(
      "#version 330 core\n"
      L_SHAREDUNIFORM
      "layout(location = 0) out vec3 ocolor;"
      "layout(location = 1) out vec3 onormal;"
      "uniform sampler2D tex;"
      "in vec2 ftexcoords;"
      "void main(){"
      "vec4 color = texture(tex,ftexcoords);"
      "if(alpha(color.a)) discard;"
      "ocolor = color.rgb;"
      "onormal = vec3(0,0,1.0);"
      "}",GL_FRAGMENT_SHADER));
  program.use();
  program.uniform("tex",*_texture);
  program.uniform("model",_transform->matrix());
  GL::quad().draw();
}
