#pragma once

#include "../math/Vector.h"
#include "../math/Matrix.h"
#include "Mesh.h"
#include "Texture.h"
#include "Program.h"

namespace L {
  namespace GL {
    void APIENTRY debug_callback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);
    const Texture& whiteTexture();
    Program& baseProgram();
    Program& baseColorProgram();
    const char* error();
    const Mesh& quad();
    const Mesh& cube();
    const Mesh& wireCube();
    const Mesh& sphere();
    const Mesh& wireSphere();
    void makeDisc(Mesh&,int slices);
    void makeSphere(Mesh&,int rec);
  }
}
