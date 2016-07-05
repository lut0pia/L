#pragma once

#include "FrameBuffer.h"
#include "RenderBuffer.h"
#include "Program.h"
#include "Texture.h"

namespace L {
  namespace GL {
    class PostProcess {
      L_NOCOPY(PostProcess)
    private:
      FrameBuffer _frameBuffer;
      Texture _color;
      RenderBuffer _depth;
      float _aspect;
    public:
      PostProcess(int width,int height);
      void resize(int width,int height);
      void prerender();
      void postrender(Program&);
    };
  }
}

