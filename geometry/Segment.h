#ifndef DEF_L_Segment
#define DEF_L_Segment

#include "../general.h"
#include "Vector.h"

namespace L{
    template <int d,class T>
    class Segment{
        protected:
            Vector<d,T> a, b;
            Segment(){}
        public:
            Segment(const Vector<d,T>& a, const Vector<d,T>& b) : a(a), b(b){}

            inline const Vector<d,T>& gA() const{return a;}
            inline const Vector<d,T>& gB() const{return b;}
    };
}

#endif
