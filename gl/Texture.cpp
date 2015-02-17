#include "Texture.h"

using namespace L;
using namespace GL;

Texture::Texture(const Image::Bitmap& bmp){
    GLubyte* pixelArray = new GLubyte[bmp.width()*bmp.height()*4];

    // Copy data
    for(size_t x=0;x<bmp.width();x++)
        for(size_t y=0;y<bmp.height();y++){
            GLubyte* tmp = pixelArray+(x*4+(y*bmp.width())*4);
            const Color& c = bmp(x,y);
            *tmp++ = c.r();
            *tmp++ = c.g();
            *tmp++ = c.b();
            *tmp = c.a();
        }

    // Configuring
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.width(), bmp.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelArray);
    delete[] pixelArray;
    glBindTexture(GL_TEXTURE_2D,0);

    width = bmp.width();
    height = bmp.height();
}
Texture::~Texture(){
    glDeleteTextures(1,&_id);
}
