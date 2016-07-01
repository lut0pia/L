#pragma once

#include "../macros.h"
#include <GL/glew.h>

namespace L {
  namespace GL {
    class Buffer {
      private:
        GLuint _id, _target;
      public:
        Buffer(GLuint target);
        ~Buffer();
        L_NOCOPY(Buffer)

        void bind();
        void data(GLsizeiptr size, const void* data, GLuint usage);
        void subData(GLintptr offset, GLsizeiptr size, const void* data);
    };
  }
}
