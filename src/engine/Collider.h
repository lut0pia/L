#pragma once

#include "Transform.h"
#include "RigidBody.h"
#include "ScriptComponent.h"
#include "../containers/IntervalTree.h"

namespace L {
  class Collider : public Component {
    L_COMPONENT(Collider)
  protected:
    static Interval3fTree<Collider*> tree;
    Interval3fTree<Collider*>::Node* _node;
    Transform* _transform;
    RigidBody* _rigidbody;
    ScriptComponent* _script;
    Vector3f _center,_radius;
    Interval3f _boundingBox;
    enum {
      Box,Sphere
    } _type;
  public:
    Collider();
    ~Collider();
    void updateComponents();
    static void subUpdateAll();
    void center(const Vector3f& center);
    void box(const Vector3f& radius);
    void sphere(float radius);
    void updateBoundingBox();
    Matrix33f inertiaTensor() const;
    void render(const Camera& camera);
    static void checkCollision(Collider& a,Collider& b);
  };
  template <> inline void subUpdateAllComponents<Collider>(){ Collider::subUpdateAll(); }
}
