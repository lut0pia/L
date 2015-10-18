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
        Vector2i curDist, thumbPos;
        Vector<2,float> value;

      public:
        Slider(Vector<2,int>, Ref<Base> thumb);

        // Setters
        void sThumb(Ref<Base>);
        void sValue(Vector<2,float>);
        // Getters
        Vector<2,float> gValue() const;

        void updateThumb();
        void updateValue();

        void dimensionsChanged(Base*,Vector<2,int>);
        void updateFromAbove(Vector<2,int>,Interval<2,int>);
        void draw(GL::Program&);
        bool event(const Window::Event& e);
    };
  }
}


#endif



