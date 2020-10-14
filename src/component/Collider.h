#pragma once

#include "Component.h"
#include "../container/IntervalTree.h"

namespace L {
  class Collider : public TComponent<Collider> {
  public:
    static Interval3fTree<Collider*> tree;
    Interval3fTree<Collider*>::Node* _node;
    class Transform* _transform;
    class RigidBody* _rigidbody;
    class ScriptComponent* _script;
    Vector3f _center, _radius;
    Interval3f _bounding_box;
    enum {
      Box, Sphere
    } _type;
    struct Collision {
      bool colliding;
      Vector3f point, normal;
      float overlap;
    };
  public:
    Collider();
    ~Collider();

    virtual void update_components() override;
    static void script_registration();

    static void custom_sub_update_all();
    void center(const Vector3f& center);
    void box(const Vector3f& radius);
    void sphere(float radius);
    void update_bounding_box();
    bool raycast_single(const Vector3f& origin, const Vector3f& direction, float& t) const;
    Matrix33f inertia_tensor() const;
    void render(const Camera& camera);
    static bool check_collision(const Collider& a, const Collider& b, Collision&);
    static Collider* raycast(const Vector3f& origin, Vector3f direction, float& t);
  };
}
