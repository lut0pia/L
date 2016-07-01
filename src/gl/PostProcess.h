#pragma once

#include "FrameBuffer.h"
#include "RenderBuffer.h"
#include "Program.h"
#include "Texture.h"

namespace L {
  namespace GL {
    class PostProcess {
      private:
        FrameBuffer _frameBuffer;
        Texture _color;
        RenderBuffer _depth;
        float _aspect;
      public:
        PostProcess(int width, int height);
        L_NOCOPY(PostProcess)
        void resize(int width, int height);
        void prerender();
        void postrender(Program&);
    };
  }
}

