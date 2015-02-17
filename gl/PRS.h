#ifndef DEF_L_PRS
#define DEF_L_PRS

#include "../geometry.h"
#include "../math.h"

// Z is up, Y is forward, X is right

namespace L{
    class PRS{
        public:
            Point<3,double> position, rotation, scale;

            PRS(Point<3,double> position = Point<3,double>(),
                Point<3,double> rotation = Point<3,double>(),
                Point<3,double> scale = Point<3,double>(1,1,1));

            void move(const Point<3,double>&);
            void relMove(const Point<3,double>&);
            void rotate(const Point<3,double>&);
            void relRotate(const Point<3,double>&);
    };
}

#endif





