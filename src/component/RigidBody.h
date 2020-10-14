#pragma once

#include "Transform.h"

namespace L {
  class RigidBody : public TComponent<RigidBody,
    ComponentFlag::UpdateAsync | ComponentFlag::SubUpdateAsync> {
  protected:
    static Vector3f _gravity;
    Transform* _transform;
    Matrix33f _inv_inertia_tensor, _inv_inertia_tensor_world;
    Vector3f _velocity, _rotation, _force, _torque, _last_position;
    Quatf _last_rotation;
    float _inv_mass, _restitution, _drag, _ang_drag;
    bool _kinematic;
  public:
    RigidBody();

    virtual void update_components() override;
    static void script_registration();

    void update_inertia_tensor();
    void update();
    void sub_update();

    inline void kinematic(bool k) { _kinematic = k; }
    inline const Vector3f& velocity() const { return _velocity; }
    inline Vector3f relative_velocity() const { return _transform->rotation().inverse().rotate(_velocity); }
    inline float mass() const { return 1.f/_inv_mass; }
    void mass(float m);
    inline float restitution() const { return _restitution; }
    inline void restitution(float r) { _restitution = r; }
    inline float drag() const { return _drag; }
    inline void drag(float d) { _drag = d; }
    inline float angular_drag() const { return _ang_drag; }
    inline void angular_drag(float d) { _ang_drag = d; }

    inline Vector3f center() const { return _transform->position(); }
    inline void add_velocity(const Vector3f& v) { _velocity += v; }
    inline void add_force(const Vector3f& f) { _force += f; }
    inline void add_relative_force(const Vector3f& f) { add_force(_transform->rotation().rotate(f)); }
    inline void add_torque(const Vector3f& t) { _torque += t; }
    inline void add_relative_torque(const Vector3f& t) { add_torque(_transform->rotation().rotate(t)); }
    inline Vector3f velocity_at(const Vector3f& offset) const { return _rotation.cross(offset)+_velocity; }

    float delta_velocity(const Vector3f& impact, const Vector3f& normal) const;
    void apply_impulse(const Vector3f& impulse, const Vector3f& offset);
    static void collision(RigidBody* a, RigidBody* b, const Vector3f& impact, const Vector3f& normal);

    static void gravity(const Vector3f& g) { _gravity = g; }
    static const Vector3f& gravity() { return _gravity; }
  };
}
