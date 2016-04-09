#ifndef DEF_L_Image_Bitmap
#define DEF_L_Image_Bitmap

#include "Color.h"
#include "../containers/MultiArray.h"

// Don't forget it's supposed to look like this
// 0-----------x
// |ooooooooooo
// |ooooooooooo
// |ooooooooooo
// y

namespace L {
  class Bitmap : public MultiArray<2,Color> {
    public:
      typedef enum {
        NEAREST,LINEAR,CUBIC
      } InterpolationType;

      inline Bitmap() : MultiArray<2,Color>() {}
      inline Bitmap(const Vector2i& s) : MultiArray<2,Color>(s) {}
      inline Bitmap(int width, int height) : MultiArray<2,Color>(width,height) {}
      Bitmap(int width, int height, const Color&);
      inline Bitmap(const Color* a, int width, int height) : MultiArray<2,Color>(a,width,height) {}
      Bitmap(const String& filePath);
      Bitmap& load(const String& filePath);
      Bitmap& save(const String& filePath);
      inline int width() const {return size(0);}
      inline int height() const {return size(1);}
      const Color& at(int x, int y) const;

      // Image treatment
      inline Color nearest(float x,float y) const {return operator()((int)x,(int)y);}
      Color linear(float,float) const;
      Color cubic(float,float) const;
      Bitmap sub(int x, int y, int width, int height) const;
      void blit(const Bitmap&, int x, int y);
      void filter(Color);
      Bitmap trim(Color) const;
      Bitmap trim(int left, int right, int top, int bottom) const;
      void scale(int width, int height, InterpolationType = CUBIC);
      void blur(int factor);
      void saturation(float percent);
  };
}

#endif


