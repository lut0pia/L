#ifndef DEF_L_GL_MeshBuilder
#define DEF_L_GL_MeshBuilder

#include "../image/Color.h"
#include "../geometry/Point.h"
#include "../macros.h"

namespace L {
  namespace GL {
    class MeshBuilder {
      private:
        byte *_vertexBuffer, *_currentVertex;
        uint _vertexBufferSize, _vertexCount;

        uint *_indexBuffer;
        uint _indexBufferSize, _indexCount;

        uint _vertexDesc, _vertexSize,_offsetVertex, _offsetColor, _offsetNormal;

      public:
        MeshBuilder();
        ~MeshBuilder();
        L_NoCopy(MeshBuilder)
        void reset(byte vertexDesc, uint maxVertices, uint maxIndices);
        void computeNormals();
        void setVertex(const Point3f&);
        void setVertexColor(const Color&);
        uint addVertex();
        void addIndex(uint);
        void addTriangle(uint,uint,uint);
        void addQuad(uint,uint,uint,uint);
        inline byte vertexDesc() const {return _vertexDesc;}
        inline byte* vertices() const {return _vertexBuffer;}
        inline uint* indices() const {return _indexBuffer;}
        inline uint vertexCount() const {return _vertexCount;}
        inline uint indexCount() const {return _indexCount;}
        inline Point3f& vertex(uint i) const {return *((Point3f*)(_offsetVertex+_vertexBuffer+_vertexSize*i));}
        inline Point3f& normal(uint i) const {return *((Point3f*)(_offsetNormal+_vertexBuffer+_vertexSize*i));}
    };
  }
}

#endif



