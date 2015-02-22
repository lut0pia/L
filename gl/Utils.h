#ifndef DEF_L_Graphics
#define DEF_L_Graphics

#include "Texture.h"
#include "../image.h"

namespace L {
  namespace GL {
    class Utils {
      public:
        static void drawAxes();
        // Color drawing
        static void draw2dLine(Point<2,float>, Point<2,float>, int size, const Color& c);
        static void draw2dColorQuad(const Vector<float>& v, const Color& c);
        static void draw2dColorTri(const Vector<float>& v, const Color& c);
        static void draw3dColorTri(const Vector<float>& v, const Color& c);
        // Texture drawing
        static void draw2dTexQuad(const Vector<float>& v, const Vector<float>& tc, const Texture&);
        static void draw2dTexTri(const Vector<float>& v, const Vector<float>& tc, const Texture&);
        static void draw3dTexTri(const Vector<float>& v, const Vector<float>& tc, const Texture&);
        // Texture handling
        static void deleteTex(GLuint tex);
    };
  }
}

#endif





