#pragma once

#include "../math/Vector.h"
#include "../math/Matrix.h"
#include "Mesh.h"
#include "Texture.h"
#include "Program.h"

namespace L {
  namespace GL {
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
