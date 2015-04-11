#ifndef DEF_L_Texture
#define DEF_L_Texture

#include <GL/glew.h>
#include "../image/Bitmap.h"

namespace L {
  namespace GL {
    class Texture {
      private:
        GLuint _id;
        int _width, _height;
      public:
        Texture();
        Texture(const Image::Bitmap&);
        ~Texture();
        L_NoCopy(Texture)
        void load(const Image::Bitmap&);
        void load(GLsizei width, GLsizei height, const void* data = NULL);
        void bind() const;
        void unbind() const;
        void parameter(GLenum name, GLint);

        inline GLuint id() const {return _id;}
        inline int width() const {return _width;}
        inline int height() const {return _height;}
    };
  }
}

#endif


