#ifndef DEF_L_GUI_Line
#define DEF_L_GUI_Line

#include <cstdlib>
#include "Sizable.h"

namespace L{
    namespace GUI{
        class Line : public Sizable{
            protected:
                Color c;
                Point2i a,b;
                bool visible;

            public:
                Line(Point<2,int>, Color);

                void updateFromAbove(Point<2,int>,Interval<2,int>);
                void draw();
        };
    }
}

#endif



