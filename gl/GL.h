#ifndef DEF_L_GL
#define DEF_L_GL

#include "Mesh.h"
#include "Texture.h"

namespace L {
  namespace GL {
    void init();
    const Texture& whiteTexture();
    const char* error();
    void drawAxes();
    void makeDisc(Mesh&, int slices);
    // Color drawing
    void draw2dLine(Point<2,float>, Point<2,float>, int size, const Color& c);
  }
}

#endif





