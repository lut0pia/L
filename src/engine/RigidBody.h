#pragma once

#include "Transform.h"

namespace L {
  class RigidBody : public Component {
    L_COMPONENT(RigidBody)
  protected:
    static Vector3f _gravity;
    Transform* _transform;
    Matrix33f _invInertiaTensor;
    Vector3f _velocity,_rotation,_force,_torque;
    float _invMass,_restitution,_drag,_angDrag;
  public:
    void start();
    void update();

    inline float mass() const{ return 1.f/_invMass; }
    inline void mass(float m){ _invMass = 1.f/m; }
    inline float restitution() const{ return _restitution; }
    inline void restitution(float r){ _restitution = r; }
    inline float drag() const{ return _drag; }
    inline void drag(float d){ _drag = d; }
    inline float angularDrag() const{ return _angDrag; }
    inline void angularDrag(float d){ _angDrag = d; }

    inline Vector3f center() const{ return _transform->absolutePosition(); }
    inline void addSpeed(const Vector3f& v){ _velocity += v; }
    inline void addForce(const Vector3f& f){ _force += f; }
    inline void addRelativeForce(const Vector3f& f){ addForce(_transform->absoluteRotation().rotate(f)); }
    inline void addTorque(const Vector3f& t){ _torque += t; }
    inline void addRelativeTorque(const Vector3f& t){ addTorque(_transform->absoluteRotation().rotate(t)); }
    inline Vector3f velocityAt(const Vector3f& offset) const{ return _rotation.cross(offset)+_velocity; }

    float deltaVelocity(const Vector3f& impact,const Vector3f& normal) const;
    void applyImpulse(const Vector3f& impulse,const Vector3f& offset);
    static void collision(RigidBody* a,RigidBody* b,const Vector3f& impact,const Vector3f& normal);

    static void gravity(const Vector3f& g){ _gravity = g; }
    static const Vector3f& gravity(){ return _gravity; }
  };
}
