#ifndef DEF_L_Image_Vector
#define DEF_L_Image_Vector

#include "../containers/Ref.h"
#include "../containers/XML.h"
#include "primitive.h"

namespace L{
    namespace Image{
        class Vector{
            private:
                double width, height;
                L::Vector<Ref<Primitive::Base> > primitives;
            public:
                Vector();
                Vector(const XML&);
                void drawOn(Image::Bitmap&) const;
        };
    }
}

#endif


