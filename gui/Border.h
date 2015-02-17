#ifndef DEF_L_GUI_Border
#define DEF_L_GUI_Border

#include <cstdlib>
#include "Layer.h"

namespace L{
    namespace GUI{
        class Border : public Layer{
            protected:
                size_t size;
                Color color;

            public:
                Border(Ref<Base>, size_t, Color);
                virtual ~Border(){}

                Point2i gPos();
                Interval2i gClip();
                Point2i gDimensions();
                void dimensionsChanged(Base*,Point<2,int>);
                void updateFromAbove(Point<2,int>,Interval<2,int>);
                void draw();
        };
    }
}

#endif



