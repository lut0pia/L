#ifndef DEF_L_Interface_obj
#define DEF_L_Interface_obj

#include <L/L.h>

namespace L {
  class OBJ : public Interface<GL::Mesh> {
    public:
      OBJ() : Interface("obj") {}

      bool from(GL::Mesh& mesh, const File& f) {
        GL::MeshBuilder mb;
        mb.reset(GL::Mesh::VERTEX|GL::Mesh::NORMAL);
        FileStream file(f.path(),"rb");
        while(!file.end()) {
          String line(file.line());
          Array<String> linePart(line.explode(' '));
          if(linePart[0]=="v") { // Vertex
            Vector3f tmp;
            tmp.x() = String::to<float>(linePart[1]);
            tmp.y() = String::to<float>(linePart[2]);
            tmp.z() = String::to<float>(linePart[3]);
            mb.setVertex(tmp);
            mb.addVertex();
          }/* else if(linePart[0]=="vt") { // TexCoord
              Point2f tmp;
              tmp.x() = FromString<float>(linePart[1]);
              tmp.y() = FromString<float>(linePart[2]);
              mesh.texCoord.push_back(tmp);
            } else if(linePart[0]=="vn") { // Normal
              Point3f tmp;
              tmp.x() = FromString<float>(linePart[1]);
              tmp.y() = FromString<float>(linePart[2]);
              tmp.z() = FromString<float>(linePart[3]);
              mesh.normal.push_back(tmp);
            }*/ else if(linePart[0]=="f") { // Face abc:vertex; def:tex; g:normal;
            if(1) {
              Array<String> linePartPart(linePart[1].explode('/'));
              mb.addIndex(String::to<int>(linePartPart[0])-1);
              //tmp.t1 = FromString<size_t>(linePartPart[1])-1;
            }
            if(1) {
              Array<String> linePartPart(linePart[2].explode('/'));
              mb.addIndex(String::to<int>(linePartPart[0])-1);
              //tmp.t2 = FromString<size_t>(linePartPart[1])-1;
            }
            if(1) {
              Array<String> linePartPart(linePart[3].explode('/'));
              mb.addIndex(String::to<int>(linePartPart[0])-1);
              //tmp.t3 = FromString<size_t>(linePartPart[1])-1;
              //tmp.n = FromString<size_t>(linePartPart[2])-1;
            }
          }
        }
        mb.computeNormals();
        reconstruct(mesh,mb);
        return true;
      }
  };
}

#endif

