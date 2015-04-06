#ifndef DEF_L_GUI_Layer
#define DEF_L_GUI_Layer

#include <cstdlib>
#include "Base.h"

namespace L {
  namespace GUI {
    class Layer : public Base {
      protected:
        Ref<Base> inner;

      public:
        Layer(const Ref<Base>&);
        virtual ~Layer() {}

        inline const Ref<Base>& gInner() {return inner;}
        virtual Point2i gPos();
        virtual Interval2i gClip();
        virtual Point2i gDimensions();
        virtual void dimensionsChanged(Base*,Point<2,int>);
        virtual void updateFromAbove(Point<2,int>,Interval<2,int>);
        virtual void draw(GL::Program&);
        virtual bool event(const Window::Event&);
    };
  }
}

#endif




