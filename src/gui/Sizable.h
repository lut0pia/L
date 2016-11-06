#pragma once

#include "Solid.h"

namespace L{
    namespace GUI{
        class Sizable : public Solid{
            protected:
                Vector2i dimensions;
            public:
                Sizable();
                Sizable(Vector<2,int>);
                virtual ~Sizable(){}

                virtual void sDimensions(Vector<2,int>);
                Vector2i gDimensions();
        };
    }
}
