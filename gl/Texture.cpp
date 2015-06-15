#include "Texture.h"

using namespace L;
using namespace GL;

Texture::Texture() : _width(0), _height(0) {
  glGenTextures(1, &_id);
}
Texture::Texture(const Image::Bitmap& bmp, bool mipmaps) {
  glGenTextures(1, &_id);
  load(bmp,mipmaps);
}
Texture::~Texture() {
  glDeleteTextures(1,&_id);
}
void Texture::load(const Image::Bitmap& bmp, bool mipmaps) {
  _width = bmp.width();
  _height = bmp.height();
  GLubyte* pixelArray = new GLubyte[_width*_height*4];
  // Copy data
  for(int x(0); x<_width; x++)
    for(int y(0); y<_height; y++) {
      GLubyte* tmp = pixelArray+(x*4+(y*_width)*4);
      const Color& c = bmp(x,y);
      *tmp++ = c.r();
      *tmp++ = c.g();
      *tmp++ = c.b();
      *tmp = c.a();
    }
  load(_width,_height,pixelArray,mipmaps);
  delete[] pixelArray;
}
void Texture::load(GLsizei width, GLsizei height, const void* data, bool mipmaps) {
  _width = width;
  _height = height;
  parameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
  parameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
  parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  parameter(GL_TEXTURE_MIN_FILTER, (mipmaps)?GL_LINEAR_MIPMAP_LINEAR:GL_LINEAR);
  if(mipmaps)
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, data);
  else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  unbind();
}
void Texture::bind() const {
  glBindTexture(GL_TEXTURE_2D, _id);
}
void Texture::unbind() const {
  glBindTexture(GL_TEXTURE_2D,0);
}
void Texture::parameter(GLenum name, GLint param) {
  bind();
  glTexParameteri(GL_TEXTURE_2D,name,param);
}
