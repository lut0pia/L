#ifndef DEF_L_GL
#define DEF_L_GL

#include "Texture.h"
#include "../image.h"

namespace L {
  namespace GL {
    void init();
    const Texture& whiteTexture();
    const char* error();
    void drawAxes();
    // Color drawing
    void draw2dLine(Point<2,float>, Point<2,float>, int size, const Color& c);
  }
}

#endif





