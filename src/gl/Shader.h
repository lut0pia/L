#pragma once

#include "GL.h"
#include "../system/File.h"
#include "../macros.h"

namespace L {
  namespace GL {
    class Shader {
      L_NOCOPY(Shader)
    private:
      GLuint _id;
      void load(const char** src, uint32_t count, GLint* lengths = nullptr);
      inline void load(const char* src) { load(&src, 1); }
    public:
      Shader(const char* src, GLenum type);
      Shader(const char** src, uint32_t count, GLenum type);
      Shader(const char** src, GLint* lengths, uint32_t count, GLenum type);
      ~Shader();
      bool check() const;
      inline GLuint id() const { return _id; }
    };
  }
}
