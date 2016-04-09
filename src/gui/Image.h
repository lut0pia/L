#ifndef DEF_L_GUI_Image
#define DEF_L_GUI_Image

#include <cstdlib>
#include "Solid.h"
#include "../gl/Texture.h"

namespace L {
  namespace GUI {
    class Image : public Solid {
      protected:
        GL::Texture _texture;
        Vector2i _dimensions;

      public:
        Image();
        Image(const Bitmap&);
        virtual ~Image() {}
        void bitmap(const Bitmap&);
        Vector2i gDimensions();
        void draw(GL::Program&);
    };
  }
}

#endif



