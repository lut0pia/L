#pragma once

#include "../math/Vector.h"
#include "../math/Matrix.h"

namespace L {
  namespace GL {
    class Mesh;
    class Texture;
    class Program;
    const Texture& whiteTexture();
    Program& baseProgram();
    const char* error();
    const Mesh& quad();
    const Mesh& cube();
    const Mesh& wireCube();
    const Mesh& wireSphere();
    void makeDisc(Mesh&,int slices);
    void makeSphere(Mesh&,int rec);
  }
}
