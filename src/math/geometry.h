#pragma once

#include "Quaternion.h"
#include "Matrix.h"
#include "Vector.h"

namespace L{
  bool lineLineIntersect(const Vector3f& p1,const Vector3f& p2,
                         const Vector3f& p3,const Vector3f& p4,
                         Vector3f* a,Vector3f* b);
  Matrix44f SQTToMat(const Quatf& q,const Vector3f& t = 0.f,float scale = 1.f);
  Matrix33f quatToMat(const Quatf& q);
}