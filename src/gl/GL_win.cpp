#include "GL.h"

using namespace L;

static void* get_gl_func(const char* name) {
  static HMODULE ogl_module(LoadLibraryA("opengl32.dll"));
  void* p = wglGetProcAddress(name);
  switch(intptr_t(p)) {
    case 0:case 1:case 2:case 3: case -1:
      p = GetProcAddress(ogl_module, name);
  }
  return p;
}

void GL::init() {
#define L_GL_FUNC(type,name) name = type(get_gl_func(#name));
#include "gl_functions.def"
#undef L_GL_FUNC
}
