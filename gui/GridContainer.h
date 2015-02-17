#ifndef DEF_L_GUI_GridContainer
#define DEF_L_GUI_GridContainer

#include <cstdlib>
#include "Base.h"
#include "../containers/Array.h"

namespace L{
    namespace GUI{
        class GridContainer : public Base{
            protected:
                //Array<2,Ref<Base> > elements;
                Point2i spacing;

            public:
                GridContainer();
                virtual ~GridContainer(){}
        };
    }
}

#endif



