#pragma once

#include <cstdlib>
#include "Solid.h"
#include "../containers/Array.h"

namespace L {
  namespace GUI {
    class ListContainer : public Solid {
      protected:
        Array<Ref<Base> > elements;
        int spacing;
        Vector2i dimensions;
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

        Vector2i gDimensions();
        void dimensionsChanged(Base*,Vector<2,int>);
        void updateFromAbove(Vector<2,int>,Interval<2,int>);
        void draw(GL::Program&);
        bool event(const Window::Event&);
    };
  }
}
