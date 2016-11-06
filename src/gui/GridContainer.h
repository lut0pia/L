#pragma once

#include <cstdlib>
#include "Base.h"
#include "../containers/MultiArray.h"

namespace L {
  namespace GUI {
    class GridContainer : public Base {
      protected:
        //MultiArray<2,Ref<Base> > elements;
        Vector2i spacing;

      public:
        GridContainer();
        virtual ~GridContainer() {}
    };
  }
}
