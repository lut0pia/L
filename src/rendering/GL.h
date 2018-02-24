#pragma once

#include <GL/gl.h>
#include <GL/glext.h>

namespace L {
  namespace GL {
    void init();
    void* load_function(const char*);
    void draw(GLenum mode, GLsizei count);
  }

  // Declare GL functions
#define L_GL_FUNC(type,name) extern type name;
#include "gl_functions.def"
#undef L_GL_FUNC
}
