#ifndef DEF_L_Interface_lbm
#define DEF_L_Interface_lbm

#include <L/L.h>
#include <iostream>

namespace L {
  class LBM : public Interface<Image::Bitmap> {
    public:
      LBM() : Interface("lbm") {}

      bool to(const Image::Bitmap& bmp, std::ostream& os) {
        return false;
      }
      bool from(Image::Bitmap& bmp, const File& file) {
        return false;
      }
  };
}

#endif

