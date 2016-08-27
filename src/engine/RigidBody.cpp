#include "RigidBody.h"

using namespace L;

Vector3f RigidBody::_gravity(0.f,0.f,.0f);

RigidBody::RigidBody() :
  _invInertiaTensor(1.f),
  _velocity(0.f),_rotation(0.f),_force(0.f),_torque(0.f),
  _invMass(1.f),_restitution(.5f),_drag(0.f),_angDrag(0.f){}
void RigidBody::updateComponents(){
  _transform = entity()->requireComponent<Transform>();
}
void RigidBody::update() {
  addForce(Engine::deltaSeconds()*_gravity/_invMass); // Apply gravity
  if(_velocity.length()>.0f){ // Apply linear drag
    Vector3f dragForce(_velocity);
    dragForce.length(-_drag*_velocity.lengthSquared());
    addForce(dragForce);
  }
  if(_rotation.length()>.0f){ // Apply angular drag
    Vector3f dragTorque(_rotation);
    dragTorque.length(-_angDrag*_rotation.lengthSquared());
    addTorque(dragTorque);
  }
  // Integrate
  const Vector3f oldVelocity(_velocity),oldRotation(_rotation);
  _velocity += _invMass*_force;
  _rotation += _invInertiaTensor*_torque;
  _transform->moveAbsolute((_velocity+oldVelocity)*Engine::deltaSeconds()*.5f);
  const Vector3f rotationAvg((_rotation+oldRotation)*.5f);
  float rotLength(rotationAvg.length());
  if(rotLength>.0f)
    _transform->rotateAbsolute(rotationAvg*(1.f/rotLength),rotLength*Engine::deltaSeconds());
  _force = _torque = 0.f; // Reset force and torque for next frame
}

float RigidBody::deltaVelocity(const Vector3f& offset,const Vector3f& normal) const{
  Vector3f torquePerUnitImpulse(offset.cross(normal));
  Vector3f rotationPerUnitImpulse(_invInertiaTensor*torquePerUnitImpulse);
  Vector3f velocityPerUnitImpulse(rotationPerUnitImpulse.cross(offset));
  float angularComponent(velocityPerUnitImpulse.dot(normal));
  return angularComponent+_invMass;
}
void RigidBody::applyImpulse(const Vector3f& impulse,const Vector3f& offset){
  addForce(impulse);
  addTorque(offset.cross(impulse));
}
void RigidBody::collision(RigidBody* a,RigidBody* b,const Vector3f& impact,const Vector3f& normal) {
  Vector3f
    arel(impact-a->center()),
    brel((b) ? impact-b->center() : 0),
    av(a->velocityAt(arel)),
    bv((b) ? b->velocityAt(brel) : 0);

  float contactVelocity((av-bv).dot(normal));
  float restitution((b) ? min(a->_restitution,b->_restitution) : a->_restitution);
  float desiredDeltaVelocity(-contactVelocity*(1.f+restitution));
  float deltaVelocity(a->deltaVelocity(arel,normal));
  if(b) deltaVelocity += b->deltaVelocity(brel,normal);
  Vector3f impulse(normal*(desiredDeltaVelocity/deltaVelocity));
  if(contactVelocity<0.f){
    a->applyImpulse(impulse,arel);
    if(b) b->applyImpulse(-impulse,brel);
  }
}
