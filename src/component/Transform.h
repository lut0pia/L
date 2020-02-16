#pragma once

#include "Component.h"
#include "../math/geometry.h"
#include "../math/Matrix.h"
#include "../math/Quaternion.h"
#include "../math/Vector.h"

namespace L {
  class Transform : public TComponent<Transform> {
  private:
    Vector3f _position;
    Quatf _rotation;
  public:
    inline Transform() : _position(0.f, 0.f, 0.f) {}

    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    inline Vector3f to_absolute(const Vector3f& v) const { return position()+_rotation.rotate(v); }
    inline Vector3f from_absolute(const Vector3f& v) const { return _rotation.inverse().rotate(v-position()); }
    inline void rotate(const Quatf& q) { _rotation = _rotation * q; }
    inline void rotate_absolute(const Quatf& q) { _rotation = q * _rotation; }
    inline void move(const Vector3f& d) { _position += _rotation.rotate(d); }
    inline void move_absolute(const Vector3f& v) { _position += v; }

    inline void position(const Vector3f& p) { _position = p; }
    inline const Vector3f& position() const { return _position; }
    inline void rotation(const Quatf& r) { _rotation = r; }
    inline const Quatf& rotation() const { return _rotation; }
    inline Vector3f right() const { return rotation().rotate(Vector3f(1.f, 0.f, 0.f)); }
    inline Vector3f forward() const { return rotation().rotate(Vector3f(0.f, 1.f, 0.f)); }
    inline Vector3f up() const { return rotation().rotate(Vector3f(0.f, 0.f, 1.f)); }
    inline Matrix44f matrix() const { return sqt_to_mat(rotation(), position()); }
  };
}
