#pragma once

#include <L/src/L.h>

namespace L {
  class OBJ : public Interface<GL::Mesh> {
  public:
    OBJ() : Interface("obj") {}

    bool from(GL::Mesh& mesh,const File& f) {
      static Array<Vector3f> vertices,normals;
      static Array<Vector2f> texcoords;
      static GL::MeshBuilder mb;
      mb.reset();
      vertices.clear();
      normals.clear();
      texcoords.clear();
      FileStream fs(f.path(),"rb");
      while(!fs.end()) {
        String line(fs.line());
        Array<String> linePart(line.explode(' '));
        if(linePart.empty())
          continue;
        else if(linePart[0]=="v")
          vertices.push(atof(linePart[1]),
                        atof(linePart[2]),
                        atof(linePart[3]));
        else if(linePart[0]=="vt")
          texcoords.push(atof(linePart[1]),
                         1.f-atof(linePart[2])); // OpenGL has y going bottom up
        else if(linePart[0]=="vn")
          normals.push(atof(linePart[1]),
                       atof(linePart[2]),
                       atof(linePart[3]));
        else if(linePart[0]=="f")
          for(uintptr_t i(1); i<linePart.size(); i++){
            Array<String> indices(linePart[i].explode('/'));
            byte vertex[256];
            size_t size(0);
            int vi(atoi(indices[0])-1);
            memcpy(vertex+size,&vertices[vi],sizeof(Vector3f));
            size += sizeof(Vector3f);
            if(indices.size()>1){
              int ti(atoi(indices[1])-1);
              memcpy(vertex+size,&texcoords[ti],sizeof(Vector2f));
              size += sizeof(Vector2f);
            }
            if(indices.size()>2){
              int ni(atoi(indices[2])-1);
              memcpy(vertex+size,&normals[ni],sizeof(Vector3f));
              size += sizeof(Vector3f);
            }
            mb.addVertex(vertex,size);
          }
      }
      mesh.load(mb,GL_TRIANGLES,{
        GL::Mesh::Attribute{0,3,GL_FLOAT,GL_FALSE,sizeof(float)*8,0},
        GL::Mesh::Attribute{1,2,GL_FLOAT,GL_FALSE,sizeof(float)*8,sizeof(float)*3},
        GL::Mesh::Attribute{2,3,GL_FLOAT,GL_FALSE,sizeof(float)*8,sizeof(float)*5},
      });
      return true;
    }
  };
}
