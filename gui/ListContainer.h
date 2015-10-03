#ifndef DEF_L_GUI_ListContainer
#define DEF_L_GUI_ListContainer

#include <cstdlib>
#include "Solid.h"
#include "../containers/Array.h"

namespace L {
  namespace GUI {
    class ListContainer : public Solid {
      protected:
        Array<Ref<Base> > elements;
        int spacing;
        Point2i dimensions;
        bool blockInsert;

      public:
        ListContainer();
        ListContainer(int spacing);
        virtual ~ListContainer() {}

        void toggleBlockInsert();
        void push_back(Ref<Base>);
        void pop_back(size_t n = 1);
        size_t size();
        void clear();

        Point2i gDimensions();
        void dimensionsChanged(Base*,Point<2,int>);
        void updateFromAbove(Point<2,int>,Interval<2,int>);
        void draw(GL::Program&);
        bool event(const Window::Event&);
    };
  }
}

#endif



