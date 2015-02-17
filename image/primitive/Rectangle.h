#ifndef DEF_L_Image_Primitive_Rectangle
#define DEF_L_Image_Primitive_Rectangle

#include "Base.h"

namespace L{
    namespace Image{
        namespace Primitive{
            class Rectangle{
                private:

                public:
                    void drawOn(Image::Bitmap&) const;
            };
        }
    }
}

#endif


