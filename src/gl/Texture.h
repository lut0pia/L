#pragma once

#include <GL/glew.h>
#include "../macros.h"

namespace L {
  class Bitmap;
  namespace GL {
    class Texture {
      L_NOCOPY(Texture)
    private:
      GLuint _id;
      int _width,_height;
    public:
      Texture();
      Texture(GLint level,GLint internalformat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,const void *pixels = nullptr);
      Texture(const Bitmap&,bool mipmaps = false);
      Texture(GLsizei width,GLsizei height,const void* data = nullptr,bool mipmaps = false);
      ~Texture();
      void load(const Bitmap&,bool mipmaps = false);
      void load(GLsizei width,GLsizei height,const void* data = nullptr,bool mipmaps = false);
      void image2D(GLint level,GLint internalformat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,const void *pixels);
      void bind() const;
      void unbind() const;
      void parameter(GLenum name,GLint);

      inline GLuint id() const { return _id; }
      inline int width() const { return _width; }
      inline int height() const { return _height; }
      inline bool empty() const { return !_width || !_height; }
    };
  }
}
