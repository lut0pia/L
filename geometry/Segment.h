#ifndef DEF_L_Segment
#define DEF_L_Segment

#include "../general.h"
#include "Point.h"

namespace L{
    template <int d,class T>
    class Segment{
        protected:
            Point<d,T> a, b;
            Segment(){}
        public:
            Segment(const Point<d,T>& a, const Point<d,T>& b) : a(a), b(b){}

            inline const Point<d,T>& gA() const{return a;}
            inline const Point<d,T>& gB() const{return b;}
    };
}

#endif
