#pragma once

#include "Transform.h"
#include "ScriptComponent.h"

namespace L {
  class RigidBody : public Component {
    L_COMPONENT(RigidBody)
  protected:
    static Vector3f _gravity;
    Transform* _transform;
    ScriptComponent* _script;
    Vector3f _velocity,_rotVel;
    float _invMass;
    Matrix33f _invInertiaTensor;
  public:
    void start();
    void update();

    inline Vector3f com() const{ return _transform->absolutePosition(); }
    inline void addSpeed(const Vector3f& v){ _velocity += v; }
    inline void addForce(const Vector3f& f){ addSpeed(f*_invMass); }
    inline void addTorque(const Vector3f& t){ _rotVel += _invInertiaTensor*t; }

    Vector3f velocityAt(const Vector3f& offset) const;
    float deltaVelocity(const Vector3f& impact,const Vector3f& normal) const;
    void applyImpulse(const Vector3f& impulse,const Vector3f& offset);
    static void collision(RigidBody* a,RigidBody* b,const Vector3f& impact,const Vector3f& normal);

    static void gravity(const Vector3f& g){ _gravity = g; }
    static const Vector3f& gravity(){ return _gravity; }
  };
}
