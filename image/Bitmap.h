#ifndef DEF_L_Bitmap
#define DEF_L_Bitmap

#include "Color.h"
#include "../containers/Array.h"
#include "../geometry.h"

// Don't forget it's supposed to look like this
// 0-----------x
// |ooooooooooo
// |ooooooooooo
// |ooooooooooo
// y

namespace L {
  namespace Image {
    class Vector;
    class Bitmap : public Array<2,Color> {
      public:
        Bitmap() : Array<2,Color>() {}
        Bitmap(size_t width, size_t height);
        Bitmap(size_t width, size_t height, const Color&);
        Bitmap(size_t width, size_t height, const Image::Vector&);
        Bitmap(const String& filePath);
        Bitmap& load(const String& filePath);
        Bitmap& save(const String& filePath);
        inline size_t width() const {return size(0);}
        inline size_t height() const {return size(1);}
        //Color operator()(double,double) const;

        // Image treatment
        Bitmap sub(size_t x, size_t y, size_t width, size_t height) const;
        Bitmap filter(Color) const;
        Bitmap trim(Color) const;
        Bitmap trim(size_t left, size_t right, size_t top, size_t bottom) const;
        void scale(size_t width, size_t height);
        void blur(int factor);
        void saturation(double percent);
        void drawTriangle(Surface<2,double>,Color);
        /*
        void drawTriangle(s2dD triangle, RGBA color);
        void drawScaledTriangle(s2dD triangle, RGBA color);
        void drawTriangle(s2dD triangle, Bitmap *tex, s2dD texCoord);
        */
    };
  }
}

#endif


