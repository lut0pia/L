#pragma once

#include "Transform.h"

namespace L {
  class RigidBody : public Component {
    L_COMPONENT(RigidBody)
  protected:
    static Vector3f _gravity;
    Transform* _transform;
    Matrix33f _invInertiaTensor, _invInertiaTensorWorld;
    Vector3f _velocity, _rotation, _force, _torque, _last_position;
    Quatf _last_rotation;
    float _invMass, _restitution, _drag, _angDrag;
    bool _kinematic;
  public:
    RigidBody();

    virtual void updateComponents() override;
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    void updateInertiaTensor();
    void update();
    void subUpdate();

    inline void kinematic(bool k) { _kinematic = k; }
    inline const Vector3f& velocity() const { return _velocity; }
    inline Vector3f relativeVelocity() const { return _transform->rotation().inverse().rotate(_velocity); }
    inline float mass() const { return 1.f/_invMass; }
    void mass(float m);
    inline float restitution() const { return _restitution; }
    inline void restitution(float r) { _restitution = r; }
    inline float drag() const { return _drag; }
    inline void drag(float d) { _drag = d; }
    inline float angularDrag() const { return _angDrag; }
    inline void angularDrag(float d) { _angDrag = d; }

    inline Vector3f center() const { return _transform->position(); }
    inline void addSpeed(const Vector3f& v) { _velocity += v; }
    inline void addForce(const Vector3f& f) { _force += f; }
    inline void addRelativeForce(const Vector3f& f) { addForce(_transform->rotation().rotate(f)); }
    inline void addTorque(const Vector3f& t) { _torque += t; }
    inline void addRelativeTorque(const Vector3f& t) { addTorque(_transform->rotation().rotate(t)); }
    inline Vector3f velocityAt(const Vector3f& offset) const { return _rotation.cross(offset)+_velocity; }

    float deltaVelocity(const Vector3f& impact, const Vector3f& normal) const;
    void applyImpulse(const Vector3f& impulse, const Vector3f& offset);
    static void collision(RigidBody* a, RigidBody* b, const Vector3f& impact, const Vector3f& normal);

    static void gravity(const Vector3f& g) { _gravity = g; }
    static const Vector3f& gravity() { return _gravity; }
  };
  template <> inline void updateAllComponents<RigidBody>() { ComponentPool<RigidBody>::async_iterate([](RigidBody& c, uint32_t) { c.update(); }, 4); }
  template <> inline void subUpdateAllComponents<RigidBody>() { ComponentPool<RigidBody>::async_iterate([](RigidBody& c, uint32_t) { c.subUpdate(); }, 4); }
}
