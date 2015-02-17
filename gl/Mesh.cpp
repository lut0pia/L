#include "Mesh.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include "../Interface.h"

using namespace L;

Mesh::Mesh(){}
Mesh::Mesh(const String& filePath){
    Interface<Mesh>::fromFile(*this,filePath);
}
void Mesh::draw(){
    glBegin(GL_TRIANGLES);
    for(size_t i=0;i<face.size();i++){
        glNormal3d(normal[face[i].n].x(),normal[face[i].n].y(),normal[face[i].n].z());
        glTexCoord2d(texCoord[face[i].t1].x(),texCoord[face[i].t1].y());  glVertex3d(vertex[face[i].v1].x(),vertex[face[i].v1].y(),vertex[face[i].v1].z());
        glTexCoord2d(texCoord[face[i].t2].x(),texCoord[face[i].t2].y());  glVertex3d(vertex[face[i].v2].x(),vertex[face[i].v2].y(),vertex[face[i].v2].z());
        glTexCoord2d(texCoord[face[i].t3].x(),texCoord[face[i].t3].y());  glVertex3d(vertex[face[i].v3].x(),vertex[face[i].v3].y(),vertex[face[i].v3].z());
    }
    glEnd();
}


/*

void L_Mesh::loadFromOBJ(String filePath){
    ifstream file(filePath.c_str(), ios::in);
    if(file){
        String line;
        Vector<String> linePart, linePartPart;
        while(getline(file, line)){
            linePart = L_Explode(line,' ');
            if(linePart[0]=="v"){ // Vertex
                buff3d.x = strtod(linePart[1].c_str(),NULL);
                buff3d.y = strtod(linePart[2].c_str(),NULL);
                buff3d.z = strtod(linePart[3].c_str(),NULL);
                vertex.push_back(buff3d);
            }
            else if(linePart[0]=="vt"){ // TexCoord
                buff2d.x = strtod(linePart[1].c_str(),NULL);
                buff2d.y = strtod(linePart[2].c_str(),NULL);
                texCoord.push_back(buff2d);
            }
            else if(linePart[0]=="vn"){ // Normal
                buff3d.x = strtod(linePart[1].c_str(),NULL);
                buff3d.y = strtod(linePart[2].c_str(),NULL);
                buff3d.z = strtod(linePart[3].c_str(),NULL);
                normal.push_back(buff3d);
            }
            else if(linePart[0]=="f"){ // Face abc:vertex; def:tex; g:normal;
                linePartPart = L_Explode(linePart[1],'/');
                buff7ul.a = atoi(linePartPart[0].c_str())-1;
                buff7ul.d = atoi(linePartPart[1].c_str())-1;

                linePartPart = L_Explode(linePart[2],'/');
                buff7ul.b = atoi(linePartPart[0].c_str())-1;
                buff7ul.e = atoi(linePartPart[1].c_str())-1;

                linePartPart = L_Explode(linePart[3],'/');
                buff7ul.c = atoi(linePartPart[0].c_str())-1;
                buff7ul.f = atoi(linePartPart[1].c_str())-1;

                buff7ul.g = atoi(linePartPart[2].c_str())-1;

                face.push_back(buff7ul);
            }
        }
    }
}
*/


