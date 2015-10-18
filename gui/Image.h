#ifndef DEF_L_GUI_Image
#define DEF_L_GUI_Image

#include <cstdlib>
#include "Solid.h"
#include "../gl/Texture.h"

namespace L {
  namespace GUI {
    class Image : public Solid {
      protected:
        Ref<GL::Texture> _texture;
        Vector2i _dimensions;

      public:
        Image();
        Image(const Ref<GL::Texture>&);
        Image(const L::Image::Bitmap&);
        virtual ~Image() {}

        void texture(const Ref<GL::Texture>&);
        void bitmap(const L::Image::Bitmap&);

        Vector2i gDimensions();
        void draw(GL::Program&);
    };
  }
}

#endif



