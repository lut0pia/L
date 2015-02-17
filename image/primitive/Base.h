#ifndef DEF_L_Image_Primitive_Base
#define DEF_L_Image_Primitive_Base

#include "../Bitmap.h"

namespace L{
    namespace Image{
        namespace Primitive{
            class Base{
                public:
                    virtual ~Base(){}
                    virtual void drawOn(Image::Bitmap&) const = 0;
            };
        }
    }
}

#endif


