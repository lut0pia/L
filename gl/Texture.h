#ifndef DEF_L_Texture
#define DEF_L_Texture

#include <GL/glew.h>
#include "../image/Bitmap.h"

namespace L {
  namespace GL {
    class Texture {
      private:
        GLuint _id;
        uint _width, _height;
      public:
        Texture(const Image::Bitmap&);
        ~Texture();
        L_NoCopy(Texture)
        void bind() const;
        inline GLuint id() const {return _id;}
        inline uint width() const {return _width;}
        inline uint height() const {return _height;}
    };
  }
}

#endif


