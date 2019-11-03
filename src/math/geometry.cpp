#include "geometry.h"

#include "math.h"

using namespace L;

bool L::line_line_intersect(const Vector3f& p1, const Vector3f& p2,
                          const Vector3f& p3, const Vector3f& p4,
                          Vector3f* a, Vector3f* b) {
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
Matrix44f L::sqt_to_mat(const Quatf& q, const Vector3f& t, float s) {
  Matrix44f wtr;
  const float& x(q.x());
  const float& y(q.y());
  const float& z(q.z());
  const float& w(q.w());
  const float x2(x*x);
  const float y2(y*y);
  const float z2(z*z);
  wtr(0, 0) = (1.f - 2.f*y2 - 2.f*z2) * s;
  wtr(0, 1) = (2.f*x*y - 2.f*z*w) * s;
  wtr(0, 2) = (2.f*x*z + 2.f*y*w) * s;
  wtr(1, 0) = (2.f*x*y + 2.f*z*w) * s;
  wtr(1, 1) = (1.f - 2.f*x2 - 2.f*z2) * s;
  wtr(1, 2) = (2.f*y*z - 2.f*x*w) * s;
  wtr(2, 0) = (2.f*x*z - 2.f*y*w) * s;
  wtr(2, 1) = (2.f*y*z + 2.f*x*w) * s;
  wtr(2, 2) = (1.f - 2.f*x2 - 2.f*y2) *s;
  wtr(3, 0) = 0.f;
  wtr(3, 1) = 0.f;
  wtr(3, 2) = 0.f;
  wtr(0, 3) = t.x();
  wtr(1, 3) = t.y();
  wtr(2, 3) = t.z();
  wtr(3, 3) = 1.f;
  return wtr;
}
Matrix33f L::quat_to_mat(const Quatf& q) {
  Matrix33f wtr;
  const float& x(q.x());
  const float& y(q.y());
  const float& z(q.z());
  const float& w(q.w());
  const float x2(x*x);
  const float y2(y*y);
  const float z2(z*z);
  wtr(0, 0) = 1.f - 2.f*y2 - 2.f*z2;
  wtr(0, 1) = 2.f*x*y - 2.f*z*w;
  wtr(0, 2) = 2.f*x*z + 2.f*y*w;
  wtr(1, 0) = 2.f*x*y + 2.f*z*w;
  wtr(1, 1) = 1.f - 2.f*x2 - 2.f*z2;
  wtr(1, 2) = 2.f*y*z - 2.f*x*w;
  wtr(2, 0) = 2.f*x*z - 2.f*y*w;
  wtr(2, 1) = 2.f*y*z + 2.f*x*w;
  wtr(2, 2) = 1.f - 2.f*x2 - 2.f*y2;
  return wtr;
}
Quatf L::mat_to_quat(const Matrix44f& m) {
  Quatf q;
  float t;
  if(m(2, 2) < 0) {
    if(m(0, 0) > m(1, 1)) {
      t = 1.f + m(0, 0) - m(1, 1) - m(2, 2);
      q = Quatf(t, m(0, 1) + m(1, 0), m(2, 0) + m(0, 2), m(1, 2) - m(2, 1));
    } else {
      t = 1.f - m(0, 0) + m(1, 1) - m(2, 2);
      q = Quatf(m(0, 1) + m(1, 0), t, m(1, 2) + m(2, 1), m(2, 0) - m(0, 2));
    }
  } else {
    if(m(0, 0) < -m(1, 1)) {
      t = 1.f - m(0, 0) - m(1, 1) + m(2, 2);
      q = Quatf(m(2, 0) + m(0, 2), m(1, 2) + m(2, 1), t, m(0, 1) - m(1, 0));
    } else {
      t = 1.f + m(0, 0) + m(1, 1) + m(2, 2);
      q = Quatf(m(1, 2) - m(2, 1), m(2, 0) - m(0, 2), m(0, 1) - m(1, 0), t);
    }
  }
  q *= 0.5f / sqrtf(t);
  return q;
}
bool L::ray_sphere_intersect(const Vector3f& c, float r, const Vector3f& o, const Vector3f& d, float& t) {
  const float radiusSqr(sqr(r));
  const Vector3f oc(o - c);
  const float ddotoc(d.dot(oc));
  const float delta(ddotoc*ddotoc-oc.lengthSquared()+radiusSqr);
  t = -ddotoc-sqrt(delta);
  return t>=0;
}
bool L::ray_box_intersect(const Interval3f& b, const Vector3f& o, const Vector3f&, float& t, const Vector3f& id) {
  const float xmin = (b.min().x() - o.x())*id.x();
  const float xmax = (b.max().x() - o.x())*id.x();
  const float ymin = (b.min().y() - o.y())*id.y();
  const float ymax = (b.max().y() - o.y())*id.y();
  const float zmin = (b.min().z() - o.z())*id.z();
  const float zmax = (b.max().z() - o.z())*id.z();
  const float tmin = max(max(min(xmin, xmax), min(ymin, ymax)), min(zmin, zmax));
  const float tmax = min(min(max(xmin, xmax), max(ymin, ymax)), max(zmin, zmax));
  t = max(0.f, tmin);
  return (tmin<tmax && tmax>0.f);
}

Matrix44f L::rotation_matrix(const Vector3f& axis, float angle) {
  Matrix44f wtr(1.f);
  if(abs(angle) < 0.000001f)
    return wtr;
  float cosi(cos(angle));
  float sinu(sin(angle));
  const float& x(axis.x());
  const float& y(axis.y());
  const float& z(axis.z());
  const float x2(x*x);
  const float y2(y*y);
  const float z2(z*z);
  wtr(0, 0) = x2+(cosi*(1-x2));
  wtr(0, 1) = (x*y*(1-cosi))-(z*sinu);
  wtr(0, 2) = (x*z*(1-cosi))+(y*sinu);
  wtr(1, 0) = (x*y*(1-cosi))+(z*sinu);
  wtr(1, 1) = y2+(cosi*(1-y2));
  wtr(1, 2) = (y*z*(1-cosi))-(x*sinu);
  wtr(2, 0) = (x*z*(1-cosi))-(y*sinu);
  wtr(2, 1) = (y*z*(1-cosi))+(x*sinu);
  wtr(2, 2) = z2+(cosi*(1-z2));
  return wtr;
}
Matrix44f L::translation_matrix(const Vector3f& vector) {
  Matrix44f wtr(1.f);
  wtr(0, 3) = vector.x();
  wtr(1, 3) = vector.y();
  wtr(2, 3) = vector.z();
  return wtr;
}
Matrix44f L::orientation_matrix(const Vector3f& newx, const Vector3f& newy, const Vector3f& newz) {
  Matrix44f wtr(1.f);
  wtr(0, 0) = newx.x();
  wtr(1, 0) = newx.y();
  wtr(2, 0) = newx.z();
  wtr(0, 1) = newy.x();
  wtr(1, 1) = newy.y();
  wtr(2, 1) = newy.z();
  wtr(0, 2) = newz.x();
  wtr(1, 2) = newz.y();
  wtr(2, 2) = newz.z();
  return wtr;
}
Matrix44f L::scale_matrix(const Vector3f& axes) {
  Matrix44f wtr(1.f);
  for(int i(0); i<3; i++)
    wtr(i, i) = axes[i];
  return wtr;
}
