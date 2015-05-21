#ifndef DEF_L_GUI_Base
#define DEF_L_GUI_Base

#include <cstdlib>
#include "../gl/Program.h"
#include "../gl/GL.h"
#include "../containers/Ref.h"
#include "../dynamic.h"
#include "../system/Window.h"

namespace L {
  namespace GUI {
    class Container;
    class Base {
      protected:
        Base* parent;

      public:
        Base();
        L_NoCopy(Base)
        virtual ~Base() {}

        virtual Point2i gPos() = 0;
        virtual Interval2i gClip() = 0;
        void sParent(Base*);
        Base* gParent() const;
        virtual Point2i gDimensions() = 0;
        void dimensionsChanged(Point2i newDim);

        void updateAll();
        void update();
        virtual void updateFromAbove(Point<2,int>,Interval<2,int>) = 0; // Position and parent clip changed
        virtual void dimensionsChanged(Base* child, Point2i dimensions); // Dimensions of child changed

        virtual void draw(GL::Program&);
        virtual bool event(const Window::Event&); // A return value of true means the event has been used
    };
  }
}

#endif

