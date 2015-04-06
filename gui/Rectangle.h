#ifndef DEF_L_GUI_Rectangle
#define DEF_L_GUI_Rectangle

#include <cstdlib>
#include "Sizable.h"

namespace L {
  namespace GUI {
    class Rectangle : public Sizable {
      public:
        Rectangle();
        Rectangle(Point2i,Color);
        virtual ~Rectangle() {}

        Color color;
        void draw(GL::Program&);
        bool event(void* e) {return false;}
    };
  }
}

#endif



