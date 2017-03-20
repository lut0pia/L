#pragma once

#include "Transform.h"
#include "RigidBody.h"
#include "ScriptComponent.h"
#include "../containers/IntervalTree.h"

namespace L {
  class Collider : public Component {
    L_COMPONENT(Collider)
  public:
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
    static void renderAll(const Camera&);
    void center(const Vector3f& center);
    void box(const Vector3f& radius);
    void sphere(float radius);
    void updateBoundingBox();
    bool raycastSingle(const Vector3f& origin,const Vector3f& direction,float& t) const;
    Matrix33f inertiaTensor() const;
    void render(const Camera& camera);
    static void checkCollision(Collider& a,Collider& b);
    static Collider* raycast(const Vector3f& origin,Vector3f direction,float& t);
  };
  template <> inline void subUpdateAllComponents<Collider>() { Collider::subUpdateAll(); }
  template <> inline void renderAllComponents<Collider>(const Camera& cam) { Collider::renderAll(cam); }
}
