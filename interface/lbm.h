#ifndef DEF_L_Interface_lbm
#define DEF_L_Interface_lbm

#include <L/L.h>
#include <iostream>

namespace L{
    class LBM : public Interface<Image::Bitmap>{
        public: LBM() : Interface("lbm"){}

        void to(const Image::Bitmap& bmp, std::ostream& os){

        }
        void from(Image::Bitmap& bmp, const File& file){

        }
    };
}

#endif

