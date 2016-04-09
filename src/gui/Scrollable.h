#ifndef DEF_L_GUI_Scrollable
#define DEF_L_GUI_Scrollable

#include <cstdlib>
#include "Sizable.h"

namespace L {
  namespace GUI {
    class Scrollable : public Sizable {
      protected:
        Vector2i modifier, childDim;
        Ref<Base> child;

      public:
        Scrollable(Vector<2,int>,Ref<Base>);

        void sModifier(Vector<2,int>);
        Vector2i gModifier() const;
        void sValue(Vector<2,float>);
        Vector<2,float> gValue() const;

        void dimensionsChanged(Base*, Vector<2,int>);
        void updateFromAbove(Vector<2,int>,Interval<2,int>);
        void draw(GL::Program&);
        bool event(const Window::Event& e);
    };
  }
}

#endif



