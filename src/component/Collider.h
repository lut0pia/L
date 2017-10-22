#pragma once

#include "Transform.h"
#include "RigidBody.h"
#include "ScriptComponent.h"
#include "../engine/Settings.h"
#include "../container/IntervalTree.h"

namespace L {
  class Collider : public Component {
    L_COMPONENT(Collider)
  public:
    static Interval3fTree<Collider*> tree;
    Interval3fTree<Collider*>::Node* _node;
    Transform* _transform;
    RigidBody* _rigidbody;
    ScriptComponent* _script;
    Vector3f _center, _radius;
    Interval3f _boundingBox;
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
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    static void sub_update_all();
    static void render_all(const Camera& cam);
    void center(const Vector3f& center);
    void box(const Vector3f& radius);
    void sphere(float radius);
    void updateBoundingBox();
    bool raycastSingle(const Vector3f& origin, const Vector3f& direction, float& t) const;
    Matrix33f inertiaTensor() const;
    void render(const Camera& camera);
    static bool checkCollision(const Collider& a, const Collider& b, Collision&);
    static Collider* raycast(const Vector3f& origin, Vector3f direction, float& t);
  };
}
