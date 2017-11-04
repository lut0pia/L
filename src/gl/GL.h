#pragma once

#include <GL/gl.h>
#include <GL/glext.h>

namespace L {
  namespace GL {
    class Mesh;
    class Program;
    void init();
    void* load_function(const char*);
    void draw(GLenum mode, GLsizei count);
    Program& baseProgram();
    Program& baseColorProgram();
    const Mesh& quad();
    const Mesh& wireCube();
    const Mesh& wireSphere();
  }

  // Declare GL functions
#define L_GL_FUNC(type,name) extern type name;
#include "gl_functions.def"
#undef L_GL_FUNC
}
