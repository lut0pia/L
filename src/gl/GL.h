#pragma once

#include "Mesh.h"
#include "Texture.h"
#include "Program.h"

namespace L {
  namespace GL {
    void init();
    const Texture& whiteTexture();
    Program& baseProgram();
    const char* error();
    void drawAxes();
    void makeDisc(Mesh&, int slices);
    void makeSphere(Mesh&, int rec);
    // Color drawing
    void draw2dLine(Vector<2,float>, Vector<2,float>, int size, const Color& c);
    inline void color(const Color& color) {
      glColor4ub(color.r(),color.g(),color.b(),color.a());
    }
  }
}
