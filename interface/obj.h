#ifndef DEF_L_Interface_obj
#define DEF_L_Interface_obj

#include <L/L.h>

namespace L{
    class OBJ : public Interface<Mesh>{
        public: OBJ() : Interface("obj"){}

        void from(Mesh& mesh, const File& f){
            std::ifstream file(f.gPath().c_str(), std::ios::in);
            if(file){
                String line;
                while(getline(file, line)){
                    List<String> linePart(line.explode(' '));
                    if(linePart[0]=="v"){ // Vertex
                        Point<3,double> tmp;
                        tmp.x() = FromString<double>(linePart[1]);
                        tmp.y() = FromString<double>(linePart[2]);
                        tmp.z() = FromString<double>(linePart[3]);
                        mesh.vertex.push_back(tmp);
                    }
                    else if(linePart[0]=="vt"){ // TexCoord
                        Point<2,double> tmp;
                        tmp.x() = FromString<double>(linePart[1]);
                        tmp.y() = FromString<double>(linePart[2]);
                        mesh.texCoord.push_back(tmp);
                    }
                    else if(linePart[0]=="vn"){ // Normal
                        Point<3,double> tmp;
                        tmp.x() = FromString<double>(linePart[1]);
                        tmp.y() = FromString<double>(linePart[2]);
                        tmp.z() = FromString<double>(linePart[3]);
                        mesh.normal.push_back(tmp);
                    }
                    else if(linePart[0]=="f"){ // Face abc:vertex; def:tex; g:normal;
                        Mesh::Face tmp;
                        ({
                            List<String> linePartPart(linePart[1].explode('/'));
                            tmp.v1 = FromString<size_t>(linePartPart[0])-1;
                            tmp.t1 = FromString<size_t>(linePartPart[1])-1;
                        });
                        ({
                            List<String> linePartPart(linePart[2].explode('/'));
                            tmp.v2 = FromString<size_t>(linePartPart[0])-1;
                            tmp.t2 = FromString<size_t>(linePartPart[1])-1;
                        });
                        ({
                            List<String> linePartPart(linePart[3].explode('/'));
                            tmp.v3 = FromString<size_t>(linePartPart[0])-1;
                            tmp.t3 = FromString<size_t>(linePartPart[1])-1;
                            tmp.n = FromString<size_t>(linePartPart[2])-1;
                        });
                        mesh.face.push_back(tmp);
                    }
                }
            }
        }
    };
}

#endif

