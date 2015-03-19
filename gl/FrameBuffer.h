#ifndef DEF_L_GL_FrameBuffer
#define DEF_L_GL_FrameBuffer

#include <GL/glew.h>

namespace L {
  namespace GL {
    class FrameBuffer {
      private:
        GLuint _id, _target;
      public:
        FrameBuffer(GLuint target);
        ~FrameBuffer();
        void bind();
    };
  }
}

#endif



