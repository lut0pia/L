#pragma once

#include <cstdlib>
#include "Layer.h"

namespace L {
  namespace GUI {
    class Background : public Layer {
      protected:
        Color color;

      public:
        Background(const Ref<Base>&, Color);
        virtual ~Background() {}

        void draw(GL::Program&);
    };
  }
}
