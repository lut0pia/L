#ifndef DEF_L_Texture
#define DEF_L_Texture

#include <GL/gl.h>
#include "../image/Bitmap.h"

namespace L{
    namespace GL{
        class Texture{
            private:
                GLuint _id;
                uint width, height;
            public:
                Texture(const Image::Bitmap&);
                L_NoCopy(Texture)
                inline GLuint id() const{return _id;}
                inline uint gWidth() const{return width;}
                inline uint gHeight() const{return height;}
                ~Texture();
        };
    }
}

#endif


