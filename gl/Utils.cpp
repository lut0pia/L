#include "Utils.h"

using namespace L;
using namespace GL;

void Utils::draw2dLine(Point<2,float> a, Point<2,float> b, int size, const Color& c){
    glColor3ub(c.r(),c.g(),c.b());

    glBegin(GL_LINES);
        glVertex2f(a.x(),a.y());
        glVertex2f(b.x(),b.y());
    glEnd();
}
void Utils::draw2dColorQuad(const Vector<float>& v, const Color& c){
    const float* vp = &v[0];
    glColor3ub(c.r(),c.g(),c.b());
    glBegin(GL_QUADS);
        while(vp<&v.back()){
            glVertex2fv(vp);
            vp+=2;
        }
    glEnd();
}
void Utils::draw2dColorTri(const Vector<float>& v, const Color& c){
    const float* vp = &v[0];
    glColor3ub(c.r(),c.g(),c.b());
    glBegin(GL_TRIANGLES);
        while(vp<&v.back()){
            glVertex2fv(vp);
            vp+=2;
        }
    glEnd();
}
void Utils::draw3dColorTri(const Vector<float>& v, const Color& c){
    const float* vp = &v.front();
    glColor3ub(c.r(),c.g(),c.b());
    glBegin(GL_TRIANGLES);
        while(vp<&v.back()){
            glVertex3fv(vp);
            vp+=3;
        }
    glEnd();
}

// Texture drawing
void Utils::draw2dTexQuad(const Vector<float>& v, const Vector<float>& tc, const Texture& tex){
    const float *vp = &v.front(), *tcp = &tc.front();
    glColor3ub(255,255,255);
    glBindTexture(GL_TEXTURE_2D,(GLuint)tex.id());
    glBegin(GL_QUADS);
        while(vp<&v.back()){
            glTexCoord2fv(tcp);
            glVertex2fv(vp);
            tcp+=2;
            vp+=2;
        }
    glEnd();
    glBindTexture(GL_TEXTURE_2D,0);
}
void Utils::draw2dTexTri(const Vector<float>& v, const Vector<float>& tc, const Texture& tex){
    const float *vp = &v.front(), *tcp = &tc.front();
    glColor3ub(255,255,255);
    glBindTexture(GL_TEXTURE_2D,(GLuint)tex.id());
    glBegin(GL_TRIANGLES);
        while(vp<&v.back()){
            glTexCoord2fv(tcp);
            glVertex2fv(vp);
            tcp+=2;
            vp+=2;
        }
    glEnd();
    glBindTexture(GL_TEXTURE_2D,0);
}
void Utils::draw3dTexTri(const Vector<float>& v, const Vector<float>& tc, const Texture& tex){
    const float *vp = &v.front(), *tcp = &tc.front();
    glColor3ub(255,255,255);
    glBindTexture(GL_TEXTURE_2D,(GLuint)tex.id());
    glBegin(GL_TRIANGLES);
        while(vp<&v.back()){
            glTexCoord2fv(tcp);
            glVertex3fv(vp);
            tcp+=2;
            vp+=3;
        }
    glEnd();
    glBindTexture(GL_TEXTURE_2D,0);
}
