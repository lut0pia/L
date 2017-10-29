#pragma once

#include <gl/GL.h>
#include <gl/glext.h>

namespace L {
  namespace GL {
    class Mesh;
    class Program;
    void init();
    void APIENTRY debug_callback(GLenum, GLenum, GLuint, GLenum, GLsizei, const char*, const void*);
    void draw(GLenum mode, GLsizei count);
    Program& baseProgram();
    Program& baseColorProgram();
    const Mesh& quad();
    const Mesh& wireCube();
    const Mesh& wireSphere();
  }

#define L_GL_FUNC(type,name) extern type name;
#include "gl_functions.def"
#undef L_GL_FUNC
}
