#ifndef DEF_L_PRS
#define DEF_L_PRS

#include "../geometry.h"
#include "../math.h"

// Z is up, Y is forward, X is right

namespace L{
    class PRS{
        public:
            Point<3,float> position, rotation, scale;

            PRS(Point3f position = Point3f(),
                Point3f rotation = Point3f(),
                Point3f scale = Point3f(1,1,1));

            void move(const Point3f&);
            void relMove(const Point3f&);
            void rotate(const Point3f&);
            void relRotate(const Point3f&);
    };
}

#endif





