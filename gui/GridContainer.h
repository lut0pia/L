#ifndef DEF_L_GUI_GridContainer
#define DEF_L_GUI_GridContainer

#include <cstdlib>
#include "Base.h"
#include "../containers/MultiArray.h"

namespace L {
  namespace GUI {
    class GridContainer : public Base {
      protected:
        //MultiArray<2,Ref<Base> > elements;
        Point2i spacing;

      public:
        GridContainer();
        virtual ~GridContainer() {}
    };
  }
}

#endif



