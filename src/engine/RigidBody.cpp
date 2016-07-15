#include "RigidBody.h"

using namespace L;

Vector3f
RigidBody::_gravity(0.f,0.f,.0f),
RigidBody::_scale(1.f,1.f,1.f);

void RigidBody::start() {
  _transform = entity()->requireComponent<Transform>();
  _script = entity()->component<ScriptComponent>();
  _invMass = 1.f;
  _restitution = .5f;
  _velocity = Vector3f(0.f,0.f,0.f);
  _rotVel = Vector3f(0.f,0.f,0.f);
  _invInertiaTensor = Matrix33f(1.f);
}
void RigidBody::update() {
  _transform->moveAbsolute(_velocity*Engine::deltaSeconds());
  float rotLength(_rotVel.length());
  if(rotLength>.001f)
    _transform->rotateAbsolute(_rotVel*(1.f/rotLength),rotLength*Engine::deltaSeconds());
  _velocity += Engine::deltaSeconds()*_gravity;
}

Vector3f RigidBody::velocityAt(const Vector3f& offset) const{
  return _rotVel.cross(offset)+_velocity;
}

float RigidBody::deltaVelocity(const Vector3f& offset,const Vector3f& normal) const{
  Vector3f torquePerUnitImpulse(offset.cross(normal));
  Vector3f rotationPerUnitImpulse(_invInertiaTensor*torquePerUnitImpulse);
  Vector3f velocityPerUnitImpulse(rotationPerUnitImpulse.cross(offset));
  float angularComponent(velocityPerUnitImpulse.dot(normal));
  return angularComponent+_invMass;
}
void RigidBody::applyImpulse(const Vector3f& impulse,const Vector3f& offset){
  Vector3f scaledImpulse(impulse*_scale);
  addForce(scaledImpulse);
  addTorque((offset*_scale).cross(scaledImpulse));
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
    auto e(ref<Table<Var,Var>>());
    (*e)[FNV1A("type")] = FNV1A("COLLISION");
    if(a->_script){
      (*e)[FNV1A("other")] = b;
      a->_script->event(e);
    }
    if(b && b->_script){
      (*e)[FNV1A("other")] = a;
      b->_script->event(e);
    }
  }
}
