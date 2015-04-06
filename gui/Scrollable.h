#ifndef DEF_L_GUI_Scrollable
#define DEF_L_GUI_Scrollable

#include <cstdlib>
#include "Sizable.h"

namespace L {
  namespace GUI {
    class Scrollable : public Sizable {
      protected:
        Point2i modifier, childDim;
        Ref<Base> child;

      public:
        Scrollable(Point<2,int>,Ref<Base>);

        void sModifier(Point<2,int>);
        Point2i gModifier() const;
        void sValue(Point<2,float>);
        Point<2,float> gValue() const;

        void dimensionsChanged(Base*, Point<2,int>);
        void updateFromAbove(Point<2,int>,Interval<2,int>);
        void draw(GL::Program&);
        bool event(const Window::Event& e);
    };
  }
}

#endif



