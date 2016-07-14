#include "geometry.h"

#include "math.h"

using namespace L;

bool L::lineLineIntersect(const Vector3f& p1,const Vector3f& p2,
                          const Vector3f& p3,const Vector3f& p4,
                          Vector3f* a,Vector3f* b){
  Vector3f p43(p4-p3);
  float ls43(p43.lengthSquared());
  if(ls43<.0001f) return false;
  Vector3f p21(p2-p1);
  float ls21(p21.lengthSquared());
  if(ls21<.0001f) return false;
  Vector3f p13(p1-p3);
  float d1343(p13.dot(p43)),
    d4321(p43.dot(p21)),
    d1321(p13.dot(p21));
  float denom(ls21*ls43-d4321*d4321);
  if(abs(denom)<.0001f) return false;
  float numer(d1343*d4321-d1321*ls43);
  float mua(numer/denom);
  float mub((d1343+d4321*mua)/ls43);

  *a = p1 + p21*mua;
  *b = p3 + p43*mub;
  return true;
}
Matrix44f L::SQTToMat(const Quatf& q,const Vector3f& t,float s){
  Matrix44f wtr;
  const float& x(q.x());
  const float& y(q.y());
  const float& z(q.z());
  const float& w(q.w());
  const float x2(x*x);
  const float y2(y*y);
  const float z2(z*z);
  wtr(0,0) = s - 2.f*y2 - 2.f*z2;
  wtr(0,1) = 2.f*x*y - 2.f*z*w;
  wtr(0,2) = 2.f*x*z + 2.f*y*w;
  wtr(1,0) = 2.f*x*y + 2.f*z*w;
  wtr(1,1) = s - 2.f*x2 - 2.f*z2;
  wtr(1,2) = 2.f*y*z - 2.f*x*w;
  wtr(2,0) = 2.f*x*z - 2.f*y*w;
  wtr(2,1) = 2.f*y*z + 2.f*x*w;
  wtr(2,2) = s - 2.f*x2 - 2.f*y2;
  wtr(3,0) = 0.f;
  wtr(3,1) = 0.f;
  wtr(3,2) = 0.f;
  wtr(0,3) = t.x();
  wtr(1,3) = t.y();
  wtr(2,3) = t.z();
  wtr(3,3) = 1.f;
  return wtr;
}