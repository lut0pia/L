#include "RigidBody.h"

#include "Collider.h"
#include "../engine/Engine.h"

using namespace L;

Vector3f RigidBody::_gravity(0.f, 0.f, .0f);

RigidBody::RigidBody() :
  _inv_inertia_tensor(1.f), _inv_inertia_tensor_world(1.f),
  _velocity(0.f), _rotation(0.f), _force(0.f), _torque(0.f),
  _inv_mass(1.f), _restitution(.5f), _drag(0.f), _ang_drag(0.f),
  _kinematic(false) {}

void RigidBody::update_components() {
  _transform = entity()->require_component<Transform>();
  _last_position = _transform->position();
  update_inertia_tensor();
}
Map<Symbol, Var> RigidBody::pack() const {
  Map<Symbol, Var> data;
  data["inv_mass"] = _inv_mass;
  data["restitution"] = _restitution;
  data["drag"] = _drag;
  data["ang_drag"] = _ang_drag;
  data["kinematic"] = _kinematic;
  return data;
}
void RigidBody::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "inv_mass", _inv_mass);
  unpack_item(data, "restitution", _restitution);
  unpack_item(data, "drag", _drag);
  unpack_item(data, "ang_drag", _ang_drag);
  unpack_item(data, "kinematic", _kinematic);
}
void RigidBody::script_registration() {
  L_COMPONENT_BIND(RigidBody, "rigidbody");
  L_SCRIPT_METHOD(RigidBody, "kinematic", 1, kinematic(c.param(0).get<bool>()));
  L_SCRIPT_METHOD(RigidBody, "mass", 1, mass(c.param(0).get<float>()));
  L_SCRIPT_METHOD(RigidBody, "restitution", 1, restitution(c.param(0).get<float>()));
  L_SCRIPT_METHOD(RigidBody, "drag", 1, drag(c.param(0).get<float>()));
  L_SCRIPT_METHOD(RigidBody, "angular_drag", 1, angular_drag(c.param(0).get<float>()));
  L_SCRIPT_RETURN_METHOD(RigidBody, "get_speed", 0, velocity());
  L_SCRIPT_RETURN_METHOD(RigidBody, "get_relative_speed", 0, relative_velocity());
  L_SCRIPT_RETURN_METHOD(RigidBody, "get_velocity_at", 1, velocity_at(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(RigidBody, "add_velocity", 1, add_velocity(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(RigidBody, "add_force", 1, add_force(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(RigidBody, "add_relative_force", 1, add_relative_force(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(RigidBody, "add_torque", 1, add_torque(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(RigidBody, "add_relative_torque", 1, add_relative_torque(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(RigidBody, "apply_impulse", 2, apply_impulse(c.param(0).get<Vector3f>(), c.param(0).get<Vector3f>()));
  ScriptContext::global(Symbol("engine_gravity")) = (ScriptNativeFunction)([](ScriptContext& c) {
    if(c.param_count()>0)
      RigidBody::gravity(c.param(0).get<Vector3f>());
    c.return_value() = RigidBody::gravity();
  });
}

void RigidBody::update_inertia_tensor() {
  Matrix33f inertia_tensor(0.f);
  int count(0);
  for(const auto& c : entity()->components()) {
    if(c.key() == Type<Collider>::description()) {
      inertia_tensor += ((Collider*)c.value().pointer())->inertia_tensor();
      count++;
    }
  }
  inertia_tensor *= mass()/count;
  _inv_inertia_tensor = inertia_tensor.inverse();
}
void RigidBody::update() {
  if(_kinematic) {
    float inv_delta(1.f/Engine::delta_seconds());
    _velocity = (_transform->position()-_last_position)*inv_delta;
    Quatf delta_quat(_transform->rotation()*_last_rotation.inverse());
    _rotation = delta_quat.to_scaled_vector()*inv_delta;
    _last_position = _transform->position();
    _last_rotation = _transform->rotation();
  } else {
    _force = _torque = 0.f; // Reset force and torque
    add_force(_gravity/_inv_mass); // Apply gravity
    if(_velocity.length()>.0f) { // Apply linear drag
      Vector3f drag_force(_velocity);
      drag_force.length(-_drag*_velocity.lengthSquared());
      add_force(drag_force);
    }
    if(_rotation.length()>.0f) { // Apply angular drag
      Vector3f drag_torque(_rotation);
      drag_torque.length(-_ang_drag*_rotation.lengthSquared());
      add_torque(drag_torque);
    }
  }
}
void RigidBody::sub_update() {
  if(!_kinematic) {
    const float delta(Engine::sub_delta_seconds());
    // Compute world inertia tensor
    const Matrix33f orientation(quat_to_mat(_transform->rotation()));
    _inv_inertia_tensor_world = orientation*_inv_inertia_tensor*orientation.transpose();

    // Integrate
    const Vector3f old_velocity(_velocity), oldRotation(_rotation);
    _velocity += (_inv_mass*_force)*delta;
    _rotation += (_inv_inertia_tensor_world*_torque)*delta;
    _transform->move_absolute((_velocity+old_velocity)*(delta*.5f));
    const Vector3f rotation_avg((_rotation+oldRotation)*.5f);
    const float rotation_length(rotation_avg.length());
    if(rotation_length>.0f)
      _transform->rotate_absolute(rotation_avg*(1.f/rotation_length), rotation_length*delta);
  }
}

void RigidBody::mass(float m) {
  _inv_mass = 1.f/m;
  update_inertia_tensor();
}

float RigidBody::delta_velocity(const Vector3f& offset, const Vector3f& normal) const {
  const Vector3f torque_per_unit_impulse(offset.cross(normal));
  const Vector3f rotation_per_unit_impulse(_inv_inertia_tensor*torque_per_unit_impulse);
  const Vector3f velocity_per_unit_impulse(rotation_per_unit_impulse.cross(offset));
  const float angular_component(velocity_per_unit_impulse.dot(normal));
  return angular_component+_inv_mass;
}
void RigidBody::apply_impulse(const Vector3f& impulse, const Vector3f& offset) {
  if(!_kinematic) {
    _velocity += _inv_mass*impulse;
    _rotation += _inv_inertia_tensor_world*offset.cross(impulse);
  }
}
void RigidBody::collision(RigidBody* a, RigidBody* b, const Vector3f& impact, const Vector3f& normal) {
  const Vector3f
    arel(impact-a->center()),
    brel(b ? impact-b->center() : 0.f),
    av(a->velocity_at(arel)),
    bv(b ? b->velocity_at(brel) : 0.f);

  const float contact_velocity((av-bv).dot(normal));
  if(contact_velocity<0.f) {
    const float restitution(contact_velocity<-.5f ? (b ? min(a->_restitution, b->_restitution) : a->_restitution) : 0.f);
    const float velocity_from_acc(b ? 0.f : _gravity.dot(normal)*Engine::sub_delta_seconds());
    const float desired_delta_velocity(-contact_velocity-restitution*(contact_velocity-velocity_from_acc));
    float delta_velocity(a->delta_velocity(arel, normal));
    if(b) delta_velocity += b->delta_velocity(brel, normal);
    const Vector3f impulse(normal*(desired_delta_velocity/delta_velocity));
    a->apply_impulse(impulse, arel);
    if(b) b->apply_impulse(-impulse, brel);
  }
}
