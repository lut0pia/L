#pragma once

#include <cstdlib>
#include "Base.h"

namespace L{
    namespace GUI{
        class Solid : public Base{
            protected:
                Vector2i pos;
                Interval2i clip;
            public:
                Solid();
                virtual ~Solid(){}

                virtual Vector2i gPos();
                virtual Interval2i gClip();
                virtual void updateFromAbove(Vector<2,int>,Interval<2,int>);
        };
    }
}
