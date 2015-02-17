#ifndef DEF_L_Mesh
#define DEF_L_Mesh

#include "../geometry.h"
#include "../stl.h"

namespace L{
    class Mesh{
        public:
            typedef struct{
                size_t v1,v2,v3,
                       t1,t2,t3,
                       n;
            } Face;

            Vector<Point<3,double> > vertex, normal;
            Vector<Point<2,double> > texCoord;
            Vector<Face> face;

            Mesh();
            Mesh(const String&);
            void draw();
            //void draw(const Skeleton&);


    };
}

#endif



