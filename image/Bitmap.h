#ifndef DEF_L_Image_Bitmap
#define DEF_L_Image_Bitmap

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
        Bitmap(int width, int height);
        Bitmap(int width, int height, const Color&);
        Bitmap(int width, int height, const Image::Vector&);
        Bitmap(const String& filePath);
        Bitmap& load(const String& filePath);
        Bitmap& save(const String& filePath);
        inline int width() const {return size(0);}
        inline int height() const {return size(1);}

        // Image treatment
        Bitmap sub(int x, int y, int width, int height) const;
        Bitmap filter(Color) const;
        Bitmap trim(Color) const;
        Bitmap trim(int left, int right, int top, int bottom) const;
        void scale(int width, int height);
        void blur(int factor);
        void saturation(float percent);
        void drawTriangle(Surface<2,float>,Color);
    };
  }
}

#endif


