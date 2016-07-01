#pragma once

#include "../macros.h"
#include <GL/glew.h>

namespace L {
  namespace GL {
    class RenderBuffer {
      private:
        GLuint _id;
      public:
        RenderBuffer();
        ~RenderBuffer();
        L_NOCOPY(RenderBuffer)
        void bind();
        void storage(GLenum internalformat, GLsizei width, GLsizei height);

        inline GLuint id() const {return _id;}
    };
  }
}
