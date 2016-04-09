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
        virtual Vector2i gPos();
        virtual Interval2i gClip();
        virtual Vector2i gDimensions();
        virtual void dimensionsChanged(Base*,Vector<2,int>);
        virtual void updateFromAbove(Vector<2,int>,Interval<2,int>);
        virtual void draw(GL::Program&);
        virtual bool event(const Window::Event&);
    };
  }
}

#endif




