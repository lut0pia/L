#ifndef DEF_Li_opengl
#define DEF_Li_opengl

/*
    Windows : opengl32
*/

#include <L/L.h>
#include <GL/GL.h>

using namespace L;

// Color drawing
void draw2dColorQuad(const vector<float>& v, const Color& c){
    const float* vp = &v[0];
    glColor3ub(c.r(),c.g(),c.b());
    glBegin(GL_QUADS);
        while(vp<&v.back()){
            glVertex2fv(vp);
            vp+=2;
        }
    glEnd();
}
void draw2dColorTri(const vector<float>& v, const Color& c){
    const float* vp = &v[0];
    glColor3ub(c.r(),c.g(),c.b());
    glBegin(GL_TRIANGLES);
        while(vp<&v.back()){
            glVertex2fv(vp);
            vp+=2;
        }
    glEnd();
}
void draw3dColorTri(const vector<float>& v, const Color& c){
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
void draw2dTexQuad(const vector<float>& v, const vector<float>& tc, void* tex){
    const float *vp = &v.front(), *tcp = &tc.front();
    glColor3ub(255,255,255);
    glBindTexture(GL_TEXTURE_2D,(GLuint)tex);
    glBegin(GL_QUADS);
        while(vp<&v.back()){
            glTexCoord2fv(tcp);
            glVertex2fv(vp);
            tcp+=2;
            vp+=2;
        }
    glEnd();
    glBindTexture(GL_TEXTURE_2D,NULL);
}
void draw2dTexTri(const vector<float>& v, const vector<float>& tc, void* tex){
    const float *vp = &v.front(), *tcp = &tc.front();
    glColor3ub(255,255,255);
    glBindTexture(GL_TEXTURE_2D,(GLuint)tex);
    glBegin(GL_TRIANGLES);
        while(vp<&v.back()){
            glTexCoord2fv(tcp);
            glVertex2fv(vp);
            tcp+=2;
            vp+=2;
        }
    glEnd();
    glBindTexture(GL_TEXTURE_2D,NULL);
}
void draw3dTexTri(const vector<float>& v, const vector<float>& tc, void* tex){
    const float *vp = &v.front(), *tcp = &tc.front();
    glColor3ub(255,255,255);
    glBindTexture(GL_TEXTURE_2D,(GLuint)tex);
    glBegin(GL_TRIANGLES);
        while(vp<&v.back()){
            glTexCoord2fv(tcp);
            glVertex3fv(vp);
            tcp+=2;
            vp+=3;
        }
    glEnd();
    glBindTexture(GL_TEXTURE_2D,NULL);
}

// Texture handling
void* createTex(const Bitmap& bmp){
    GLuint wtr;
    size_t h = bmp.gHeight(), w = bmp.gWidth();
    const GLubyte* pixelArray;
    GLenum format;

    // Format
    switch(bmp.gColorFormat()){
        case _cfGS:     format = GL_INTENSITY8; break;
        case _cfRGB:    format = GL_RGB; break;
        case _cfRGBA:   format = GL_RGBA; break;
    }

    pixelArray = bmp.gData();

    // Configuring
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &wtr);
    glBindTexture(GL_TEXTURE_2D, wtr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixelArray);
    glBindTexture(GL_TEXTURE_2D,NULL);

    return (void*)wtr;
}
void deleteTex(void* tex){
    glDeleteTextures(1,(GLuint*)&tex);
}

void Li_opengl(){
    // Color drawing
    Graphics::draw2dColorQuad = draw2dColorQuad;
    Graphics::draw2dColorTri = draw2dColorTri;
    Graphics::draw3dColorTri = draw3dColorTri;
    // Texture drawing
    Graphics::draw2dTexQuad = draw2dTexQuad;
    Graphics::draw2dTexTri = draw2dTexTri;
    Graphics::draw3dTexTri = draw3dTexTri;
    // Texture handling
    Graphics::createTex = createTex;
    Graphics::deleteTex = deleteTex;
}

#endif
