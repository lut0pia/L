#pragma once

#include <GL/glew.h>
#include "../system/File.h"
#include "../macros.h"

namespace L {
  namespace GL {
    class Shader {
      L_NOCOPY(Shader)
    private:
      GLuint _id;
      void load(const char* src);
    public:
      Shader(File file,GLenum type);
      Shader(const char* src,GLenum type);
      ~Shader();
      inline GLuint id() const { return _id; }
    };
  }
}
