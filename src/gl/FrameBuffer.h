#ifndef DEF_L_GL_FrameBuffer
#define DEF_L_GL_FrameBuffer

#include "RenderBuffer.h"
#include "Texture.h"

namespace L {
  namespace GL {
    class FrameBuffer {
      private:
        GLuint _id, _target;
      public:
        FrameBuffer(GLuint target);
        ~FrameBuffer();
        L_NoCopy(FrameBuffer)
        void bind();
        void unbind();
        void attach(GLenum attachment, const RenderBuffer&);
        void attach(GLenum attachment, const Texture&);
    };
  }
}

#endif



