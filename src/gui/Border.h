#pragma once

#include <cstdlib>
#include "Layer.h"

namespace L {
  namespace GUI {
    class Border : public Layer {
      protected:
        size_t size;
        Color color;

      public:
        Border(Ref<Base>, size_t, Color);
        virtual ~Border() {}

        Vector2i gPos();
        Interval2i gClip();
        Vector2i gDimensions();
        void dimensionsChanged(Base*,Vector<2,int>);
        void updateFromAbove(Vector<2,int>,Interval<2,int>);
        void draw(GL::Program&);
    };
  }
}
