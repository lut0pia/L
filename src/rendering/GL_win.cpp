#include "GL.h"

using namespace L;

void* GL::load_function(const char* name) {
  static HMODULE ogl_module(LoadLibraryA("opengl32.dll"));
  void* p = wglGetProcAddress(name);
  switch(intptr_t(p)) {
    case 0:case 1:case 2:case 3: case -1:
      p = GetProcAddress(ogl_module, name);
  }
  return p;
}
