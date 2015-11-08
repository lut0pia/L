#ifndef DEF_L_Image_Bitmap
#define DEF_L_Image_Bitmap

#include "Color.h"
#include "../containers/MultiArray.h"
#include "../geometry.h"

// Don't forget it's supposed to look like this
// 0-----------x
// |ooooooooooo
// |ooooooooooo
// |ooooooooooo
// y

namespace L {
  class Bitmap : public MultiArray<2,Color> {
    public:
      Bitmap() : MultiArray<2,Color>() {}
      Bitmap(int width, int height);
      Bitmap(int width, int height, const Color&);
      inline Bitmap(const Color* a, int width, int height) : MultiArray<2,Color>(a,width,height) {}
      Bitmap(const String& filePath);
      Bitmap& load(const String& filePath);
      Bitmap& save(const String& filePath);
      inline int width() const {return size(0);}
      inline int height() const {return size(1);}

      // Image treatment
      Bitmap sub(int x, int y, int width, int height) const;
      void blit(const Bitmap&, int x, int y);
      Bitmap filter(Color) const;
      Bitmap trim(Color) const;
      Bitmap trim(int left, int right, int top, int bottom) const;
      void scale(int width, int height);
      void blur(int factor);
      void saturation(float percent);
      void drawTriangle(Surface<2,float>,Color);
  };
}

#endif


