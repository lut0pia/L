#include "Texture.h"

#include "../image/Bitmap.h"

using namespace L;
using namespace GL;

Texture::Texture() : _width(0), _height(0) {
  glCreateTextures(GL_TEXTURE_2D, 1, &_id);
}
Texture::Texture(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) : Texture() {
  image2D(level, internalformat, width, height, border, format, type, pixels);
}
Texture::Texture(const Bitmap& bmp) : Texture() {
  load(bmp);
}
Texture::Texture(GLsizei width, GLsizei height, const void* data) : Texture() {
  load(width, height, data);
}
Texture::~Texture() {
  glDeleteTextures(1, &_id);
}
void Texture::load(const Bitmap& bmp) {
  load(bmp.width(), bmp.height(), &bmp[0]);
}
void Texture::load(GLsizei width, GLsizei height, const void* data) {
  parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  image2D(0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}
void Texture::image2D(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
  _width = width;
  _height = height;
  glTextureImage2DEXT(_id, GL_TEXTURE_2D, level, internalformat, width, height, border, format, type, pixels);
}
void Texture::subload(const Bitmap& bmp, GLint x, GLint y) {
  subimage2D(0, x, y, bmp.width(), bmp.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, &bmp[0]);
}
void Texture::subimage2D(GLint level, GLint x, GLint y, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
  L_ASSERT(x+width<=_width && y+height<=_height);
  glTextureSubImage2D(_id, level, x, y, width, height, format, type, pixels);
}
void Texture::bind() const {
  glBindTexture(GL_TEXTURE_2D, _id);
}
void Texture::unbind() const {
  glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::parameter(GLenum name, GLint param) {
  glTextureParameteri(_id, name, param);
}
void Texture::generate_mipmap() {
  parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateTextureMipmap(_id);
}
