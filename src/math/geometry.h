#pragma once

#include "math.h"
#include "Vector.h"

namespace L{
  inline bool lineLineIntersect(const Vector3f& p1,const Vector3f& p2,
                                const Vector3f& p3,const Vector3f& p4,
                                Vector3f* a,Vector3f* b){
    Vector3f p43(p4-p3);
    float ls43(p43.lengthSquared());
    if(ls43<.001f) return false;
    Vector3f p21(p2-p1);
    float ls21(p21.lengthSquared());
    if(ls21<.001f) return false;
    Vector3f p13(p1-p3);
    float d1343(p13.dot(p43)),
      d4321(p43.dot(p21)),
      d1321(p13.dot(p21));
    float denom(ls21*ls43-d4321*d4321);
    if(abs(denom)<.001f) return false;
    float numer(d1343*d4321-d1321*ls43);
    float mua(numer/denom);
    float mub((d1343+d4321*mua)/ls43);

    *a = p1 + p21*mua;
    *b = p3 + p43*mub;
    return true;
  }
}