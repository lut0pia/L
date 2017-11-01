#pragma once

#include <initializer_list>
#include "GL.h"
#include "../macros.h"

namespace L {
  namespace GL {
    class Texture;
    class FrameBuffer {
      L_NOCOPY(FrameBuffer)
    private:
      GLuint _id;
    public:
      FrameBuffer();
      FrameBuffer(const std::initializer_list<Texture*>& colors, Texture* depth = nullptr);
      ~FrameBuffer();
      void bind();
      void unbind();
      void attach(GLenum attachment, const Texture&);
      bool check();
    };
  }
}
