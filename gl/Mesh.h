#ifndef DEF_L_GL_Mesh
#define DEF_L_GL_Mesh

#include "Buffer.h"
#include "MeshBuilder.h"
#include "../macros.h"

namespace L {
  namespace GL {
    class Mesh {
      public:
        static const byte VERTEX = 0x01;
        static const byte COLOR = 0x02;
        static const byte NORMAL = 0x04;
        static const byte TEXCOORD = 0x08;
      private:
        byte _vertexDesc;
        Buffer _vertexBuffer, _indexBuffer;
        GLsizei _vertexCount, _vertexSize, _indexCount;
        GLenum _primitive;
      public:
        Mesh();
        Mesh(const String&);
        Mesh(const MeshBuilder&, GLenum primitive = GL_TRIANGLES);
        //Mesh(byte vertexDesc, float* vertices, GLsizei count, GLenum primitive = GL_TRIANGLES);
        L_NoCopy(Mesh)
        void draw();

        static GLsizei vertexSize(byte desc);
        static GLsizei attributeOffset(byte desc, byte type);
    };
  }
}

#endif



