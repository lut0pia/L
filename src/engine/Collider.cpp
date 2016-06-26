#include "Collider.h"

#include "../gl/GL.h"
#include "../math/geometry.h"

using namespace L;

void Collider::start() {
  _transform = entity()->requireComponent<Transform>();
  _rigidbody = entity()->component<RigidBody>();
}
Interval1f Collider::project(const Vector3f& axis) const {
  Interval1f wtr;
  switch(_type) {
    case Box:
      for(int i(0); i<8; i++) {
        float p(axis.dot(Vector3f(_transform->toAbsolute(_box.corner(i)))));
        if(i)
          wtr.add(p);
        else
          wtr = Interval1f(p);
      }
      break;
    case Sphere:
    {
      float pcenter(axis.dot(_transform->toAbsolute(_center)));
      wtr = Interval1f(pcenter-_radius,pcenter+_radius);
    }
    break;
  }
  return wtr;
}
Interval3f Collider::boundingBox() const {
  switch(_type) {
    case Box:
      return _box.transformed(_transform->matrix());
    case Sphere:
    {
      Vector3f center(_transform->toAbsolute(_center));
      return Interval3f(center-_radius,center+_radius);
    }
    default:
      return Interval3f();
  }
}
Vector3f Collider::leastToAxis(const Vector3f& axis) const {
  Vector3f wtr;
  switch(_type) {
    case Box:
    {
      float leastProjected;
      for(int i(0); i<8; i++) {
        Vector3f p(_transform->toAbsolute(_box.corner(i)));
        float projected(axis.dot(p));
        if(!i || projected<leastProjected) {
          wtr = p;
          leastProjected = projected;
        }
      }
    }
    break;
    case Sphere:
      wtr = _transform->toAbsolute(_center)-axis*_radius;
      break;
  }
  return wtr;
}
void Collider::render(const Camera& camera) {
  glPushMatrix();
  glMultTransposeMatrixf(_transform->matrix().array());
  GL::whiteTexture().bind();
  GL::color(Color::red);
  glBegin(GL_LINES);
  // X
  glVertex3f(_box.min().x(),_box.min().y(),_box.min().z());
  glVertex3f(_box.max().x(),_box.min().y(),_box.min().z());
  glVertex3f(_box.min().x(),_box.max().y(),_box.min().z());
  glVertex3f(_box.max().x(),_box.max().y(),_box.min().z());
  glVertex3f(_box.min().x(),_box.min().y(),_box.max().z());
  glVertex3f(_box.max().x(),_box.min().y(),_box.max().z());
  glVertex3f(_box.min().x(),_box.max().y(),_box.max().z());
  glVertex3f(_box.max().x(),_box.max().y(),_box.max().z());
  // Y
  glVertex3f(_box.min().x(),_box.min().y(),_box.min().z());
  glVertex3f(_box.min().x(),_box.max().y(),_box.min().z());
  glVertex3f(_box.max().x(),_box.min().y(),_box.min().z());
  glVertex3f(_box.max().x(),_box.max().y(),_box.min().z());
  glVertex3f(_box.min().x(),_box.min().y(),_box.max().z());
  glVertex3f(_box.min().x(),_box.max().y(),_box.max().z());
  glVertex3f(_box.max().x(),_box.min().y(),_box.max().z());
  glVertex3f(_box.max().x(),_box.max().y(),_box.max().z());
  // Z
  glVertex3f(_box.min().x(),_box.min().y(),_box.min().z());
  glVertex3f(_box.min().x(),_box.min().y(),_box.max().z());
  glVertex3f(_box.max().x(),_box.min().y(),_box.min().z());
  glVertex3f(_box.max().x(),_box.min().y(),_box.max().z());
  glVertex3f(_box.min().x(),_box.max().y(),_box.min().z());
  glVertex3f(_box.min().x(),_box.max().y(),_box.max().z());
  glVertex3f(_box.max().x(),_box.max().y(),_box.min().z());
  glVertex3f(_box.max().x(),_box.max().y(),_box.max().z());
  glEnd();
  glPopMatrix();
}
void Collider::checkCollision(const Collider& a,const Collider& b) {
  if(a._type==Box && b._type==Box) {
    const Transform *at(a._transform),*bt(b._transform);
    Vector3f ar(at->right()),af(at->forward()),au(at->up()),
      br(bt->right()),bf(bt->forward()),bu(bt->up());
    Vector3f axes[] = {ar,af,au,br,bf,bu,
                       ar.cross(br).normalized(),
                       ar.cross(bf).normalized(),
                       ar.cross(bu).normalized(),
                       af.cross(br).normalized(),
                       af.cross(bf).normalized(),
                       af.cross(bu).normalized(),
                       au.cross(br).normalized(),
                       au.cross(bf).normalized(),
                       au.cross(bu).normalized()
    };
    uintptr_t axis(-1);
    float minOverlap;
    Vector3f normal;
    for(uintptr_t i(0); i<sizeof(axes)/sizeof(Vector3f); i++) {
      if(axes[i].lengthSquared()>0.001f) { // The axis is not a null vector (caused by a cross product)
        Interval1f axisA(a.project(axes[i])),axisB(b.project(axes[i])),intersection(axisA,axisB); // Compute projections and intersection
        float overlap(intersection.size().x());
        if(overlap>0.f) {
          if(axis==-1 || overlap<minOverlap) { // First or smallest overlap yet
            normal = (axisA.center().x()<axisB.center().x()) ? -axes[i] : axes[i];
            minOverlap = overlap;
            axis = i;
          }
        } else return; // No overlap means no collision
      }
    }
    // Resolve interpenetration
    if(b._rigidbody){
      a._transform->moveAbsolute(normal*minOverlap*.5f);
      b._transform->moveAbsolute(normal*minOverlap*-.5f);
    }
    else a._transform->moveAbsolute(normal*minOverlap);
    // Compute impact point
    Vector3f impactPoint(0,0,0);
    if(axis<6)
      impactPoint = (axis<3) ? b.leastToAxis(-normal) : a.leastToAxis(normal);
    else{
      Vector3f avertex(a.leastToAxis(normal)),bvertex(b.leastToAxis(-normal));
      const Vector3f& aaxis(axes[(axis-6)/3]),baxis(axes[((axis-6)%3)+3]);
      if(!lineLineIntersect(avertex,avertex+aaxis,bvertex,bvertex+baxis,&avertex,&bvertex))
        return; // Unable to compute intersection
      impactPoint = (avertex+bvertex)/2.f;
    }
    RigidBody::collision(a._rigidbody,b._rigidbody,impactPoint,normal);
  }
}
