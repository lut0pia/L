#pragma once

#include "Interval.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "Vector.h"

namespace L{
  bool lineLineIntersect(const Vector3f& p1,const Vector3f& p2,
                         const Vector3f& p3,const Vector3f& p4,
                         Vector3f* a,Vector3f* b);
  Matrix44f SQTToMat(const Quatf& q,const Vector3f& t = 0.f,float scale = 1.f);
  Matrix33f quatToMat(const Quatf& q);
  bool raySphereIntersect(const Vector3f& center,float radius,const Vector3f& origin,const Vector3f& direction,float& t);
  bool rayBoxIntersect(const Interval3f& box,const Vector3f& origin,const Vector3f& direction,float& t,const Vector3f& inverseDirection);
  inline bool rayBoxIntersect(const Interval3f& box,const Vector3f& origin,const Vector3f& direction,float& t){
    return rayBoxIntersect(box,origin,direction,t,Vector3f(1.f/direction.x(),1.f/direction.y(),1.f/direction.z()));
  }

  Matrix44f rotation_matrix(const Vector3f& axis, float angle);
  Matrix44f translation_matrix(const Vector3f& vector);
  Matrix44f orientation_matrix(const Vector3f& newx, const Vector3f& newy, const Vector3f& newz);
  Matrix44f scale_matrix(const Vector3f& axes);
}
