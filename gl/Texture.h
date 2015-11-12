#ifndef DEF_L_Texture
#define DEF_L_Texture

#include <GL/glew.h>
#include "../image/Bitmap.h"
#include "../macros.h"

namespace L {
  namespace GL {
    class Texture {
      private:
        GLuint _id;
        int _width, _height;
      public:
        Texture();
        Texture(const Bitmap&, bool mipmaps = false);
        ~Texture();
        L_NoCopy(Texture)
        void load(const Bitmap&, bool mipmaps = false);
        void load(GLsizei width, GLsizei height, const void* data = NULL, bool mipmaps = false);
        void bind() const;
        void unbind() const;
        void parameter(GLenum name, GLint);

        inline GLuint id() const {return _id;}
        inline int width() const {return _width;}
        inline int height() const {return _height;}
        inline bool empty() const {return !_width || !_height;}
    };
  }
}

#endif


