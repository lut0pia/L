#pragma once

#include "../image/Color.h"
#include "../math/Vector.h"

namespace L {
  namespace GL {
    class MeshBuilder {
      private:
        Array<byte> _vertexBuffer;
        Array<uint32_t> _indexBuffer;
        byte _vertexTmp[128];

        byte _vertexDesc;
        uint32_t _vertexSize, _offsetVertex, _offsetColor, _offsetNormal;

      public:
        void reset(byte vertexDesc);
        void computeNormals();
        void setVertex(const Vector3f&);
        void setVertexColor(const Color&);
        uint32_t addVertex();
        void addIndex(uint32_t);
        void addTriangle(uint32_t,uint32_t,uint32_t);
        void addQuad(uint32_t,uint32_t,uint32_t,uint32_t);
        inline byte vertexDesc() const {return _vertexDesc;}
        inline const byte* vertices() const {return &_vertexBuffer[0];}
        inline const uint32_t* indices() const {return &_indexBuffer[0];}
        inline uint32_t vertexCount() const {return _vertexBuffer.size()/_vertexSize;}
        inline uint32_t indexCount() const {return _indexBuffer.size();}
        inline Vector3f& vertex(uint32_t i) const {return *((Vector3f*)(_offsetVertex+vertices()+_vertexSize*i));}
        inline Vector3f& normal(uint32_t i) const {return *((Vector3f*)(_offsetNormal+vertices()+_vertexSize*i));}
    };
  }
}
