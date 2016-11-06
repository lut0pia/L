#pragma once

#include <cstdlib>
#include "Sizable.h"

namespace L{
    namespace GUI{
        class Line : public Sizable{
            protected:
                Color c;
                Vector2i a,b;
                bool visible;

            public:
                Line(Vector<2,int>, Color);

                void updateFromAbove(Vector<2,int>,Interval<2,int>);
                void draw();
        };
    }
}
