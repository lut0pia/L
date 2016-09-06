#pragma once

#include "Transform.h"
#include "RigidBody.h"
#include "ScriptComponent.h"
#include "../math/Interval.h"

namespace L {
  class Collider : public Component {
    L_COMPONENT(Collider)
  protected:
    Transform* _transform;
    RigidBody* _rigidbody;
    ScriptComponent* _script;
    Vector3f _center,_radius;
    mutable Interval3f _boundingBox;
    mutable uint32_t _updateFrame;
    enum {
      Box,Sphere
    } _type;
  public:
    Collider();
    void updateComponents();
    void update();
    inline void box(const Vector3f& center,const Vector3f& radius) { _type = Box; _center = center; _radius = radius; }
    inline void sphere(const Vector3f& center,float radius) { _type = Sphere; _center = center; _radius = radius; }
    Interval3f boundingBox() const;
    Matrix33f inertiaTensor() const;
    void render(const Camera& camera);
    void checkCollision(Collider& a,Collider& b);
  };
}
