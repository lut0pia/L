#ifndef DEF_L_GL_RenderBuffer
#define DEF_L_GL_RenderBuffer

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
        L_NoCopy(RenderBuffer)
        void bind();
        void storage(GLenum internalformat, GLsizei width, GLsizei height);

        inline GLuint id() const {return _id;}
    };
  }
}

#endif



