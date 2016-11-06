#pragma once

#include <cstdlib>
#include "Sizable.h"

namespace L {
  namespace GUI {
    class Rectangle : public Sizable {
      public:
        Rectangle();
        Rectangle(Vector2i,Color);
        virtual ~Rectangle() {}

        Color color;
        void draw(GL::Program&);
        bool event(void* e) {return false;}
    };
  }
}
