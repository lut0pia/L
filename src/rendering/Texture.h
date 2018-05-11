#pragma once

#include "../engine/Resource.h"
#include "GL.h"
#include "../macros.h"

namespace L {
  class Texture {
    L_NOCOPY(Texture)
  private:
    GLuint _id;
    int _width, _height;
  public:
    Texture();
    Texture(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels = nullptr);
    Texture(GLsizei width, GLsizei height, const void* data = nullptr);
    ~Texture();
    void load(GLsizei width, GLsizei height, const void* data = nullptr);
    void image2D(GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels = nullptr);
    void subimage2D(GLint level, GLint x, GLint y, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
    void bind() const;
    void unbind() const;
    void parameter(GLenum name, GLint);
    void generate_mipmap();

    inline GLuint id() const { return _id; }
    inline int width() const { return _width; }
    inline int height() const { return _height; }
    inline bool empty() const { return !_width || !_height; }
  };
  template <> void post_load_resource(ResourceSlot<Texture>& slot);
}
