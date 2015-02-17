#ifndef DEF_L_GUI_Rectangle
#define DEF_L_GUI_Rectangle

#include <cstdlib>
#include "Sizable.h"

namespace L{
    namespace GUI{
        class Rectangle : public Sizable{
            public:
                Rectangle();
                Rectangle(Point<2,int>,Color);
                virtual ~Rectangle(){}

                Color color;
                void draw();
                bool event(void* e){return false;}
        };
    }
}

#endif



