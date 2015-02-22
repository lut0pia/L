#ifndef DEF_L_GL_MeshBuilder
#define DEF_L_GL_MeshBuilder

#include "../macros.h"

namespace L {
  namespace GL {
    class MeshBuilder {
      private:
        float* _vertexBuffer;
        uint _vertexBufferSize, _vertexCount;
        uint* _indexBuffer;
        uint _indexBufferSize, _indexCount;
        float _currentVertex[32];
        uint _vertexDesc, _vertexSize;
      public:
        MeshBuilder();
        ~MeshBuilder();
        void reset(byte vertexDesc, uint maxVertices, uint maxIndices=0);
        void setVertex(float x,float y,float z);
        void setVertexColor(float r,float g,float b, float a=1);
        uint addVertex();
        void addIndex(uint);
        void addTriangle(uint,uint,uint);
        void addQuad(uint,uint,uint,uint);
        byte vertexDesc() const {return _vertexDesc;}
        float* vertices() const {return _vertexBuffer;}
        uint* indices() const {return _indexBuffer;}
        uint vertexCount() const {return _vertexCount;}
        uint indexCount() const {return _indexCount;}
    };
  }
}

#endif



