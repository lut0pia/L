#ifndef DEF_L_GUI_Image
#define DEF_L_GUI_Image

#include <cstdlib>
#include "Solid.h"
#include "../gl/Texture.h"

namespace L {
  namespace GUI {
    class Image : public Solid {
      protected:
        Ref<GL::Texture> tex;
        Point2i dimensions;

      public:
        Image();
        Image(const L::Image::Bitmap&);
        virtual ~Image() {}

        void sBitmap(const L::Image::Bitmap&);

        Point2i gDimensions();
        void draw(GL::Program&);
    };
  }
}

#endif



