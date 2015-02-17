#ifndef DEF_L_GUI_Solid
#define DEF_L_GUI_Solid

#include <iostream>
#include <cstdlib>
#include "Base.h"

namespace L{
    namespace GUI{
        class Solid : public Base{
            protected:
                Point2i pos;
                Interval2i clip;
            public:
                Solid();
                virtual ~Solid(){}

                virtual Point2i gPos();
                virtual Interval2i gClip();
                virtual void updateFromAbove(Point<2,int>,Interval<2,int>);
        };
    }
}

#endif

