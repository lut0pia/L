#pragma once

#include "../math/Vector.h"
#include "../math/Matrix.h"
#include "Mesh.h"
#include "Texture.h"
#include "Program.h"

namespace L {
  namespace GL {
    void APIENTRY debug_callback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);
    void draw(GLenum mode, GLsizei count);
    Program& baseProgram();
    Program& baseColorProgram();
    const Mesh& quad();
    const Mesh& wireCube();
    const Mesh& wireSphere();
  }
}
