#ifndef DEF_L_GL_MeshBuilder
#define DEF_L_GL_MeshBuilder

#include "../image/Color.h"
#include "../math/Vector.h"

namespace L {
  namespace GL {
    class MeshBuilder {
      private:
        Array<byte> _vertexBuffer;
        Array<uint> _indexBuffer;
        byte _vertexTmp[128];

        byte _vertexDesc;
        uint _vertexSize, _offsetVertex, _offsetColor, _offsetNormal;

      public:
        void reset(byte vertexDesc);
        void computeNormals();
        void setVertex(const Vector3f&);
        void setVertexColor(const Color&);
        uint addVertex();
        void addIndex(uint);
        void addTriangle(uint,uint,uint);
        void addQuad(uint,uint,uint,uint);
        inline byte vertexDesc() const {return _vertexDesc;}
        inline const byte* vertices() const {return &_vertexBuffer[0];}
        inline const uint* indices() const {return &_indexBuffer[0];}
        inline uint vertexCount() const {return _vertexBuffer.size()/_vertexSize;}
        inline uint indexCount() const {return _indexBuffer.size();}
        inline Vector3f& vertex(uint i) const {return *((Vector3f*)(_offsetVertex+vertices()+_vertexSize*i));}
        inline Vector3f& normal(uint i) const {return *((Vector3f*)(_offsetNormal+vertices()+_vertexSize*i));}
    };
  }
}

#endif



