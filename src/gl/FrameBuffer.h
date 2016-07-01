#pragma once

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
        L_NOCOPY(FrameBuffer)
        void bind();
        void unbind();
        void attach(GLenum attachment, const RenderBuffer&);
        void attach(GLenum attachment, const Texture&);
    };
  }
}
