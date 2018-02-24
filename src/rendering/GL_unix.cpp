#include "GL.h"

#include <GL/glx.h>

using namespace L;

void* GL::load_function(const char* name) {
  return (void*)glXGetProcAddressARB((const GLubyte*)name);
}
