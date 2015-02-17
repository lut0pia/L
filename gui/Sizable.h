#ifndef DEF_L_GUI_Sizable
#define DEF_L_GUI_Sizable

#include "Solid.h"

namespace L{
    namespace GUI{
        class Sizable : public Solid{
            protected:
                Point2i dimensions;
            public:
                Sizable();
                Sizable(Point<2,int>);
                virtual ~Sizable(){}

                virtual void sDimensions(Point<2,int>);
                Point2i gDimensions();
        };
    }
}

#endif


