#ifndef DEF_L_GUI_Slider
#define DEF_L_GUI_Slider

#include <cstdlib>
#include "Sizable.h"

namespace L {
  namespace GUI {
    class Slider : public Sizable {
      protected:
        Ref<Base> thumb;
        bool inMotion;
        Point2i curDist, thumbPos;
        Point<2,float> value;

      public:
        Slider(Point<2,int>, Ref<Base> thumb);

        // Setters
        void sThumb(Ref<Base>);
        void sValue(Point<2,float>);
        // Getters
        Point<2,float> gValue() const;

        void updateThumb();
        void updateValue();

        void dimensionsChanged(Base*,Point<2,int>);
        void updateFromAbove(Point<2,int>,Interval<2,int>);
        void draw(GL::Program&);
        bool event(const Window::Event& e);
    };
  }
}


#endif



