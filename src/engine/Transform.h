#pragma once

#include "Component.h"
#include "../math/geometry.h"
#include "../math/Matrix.h"
#include "../math/Quaternion.h"
#include "../math/Vector.h"

namespace L {
  class Transform : public Component {
    L_COMPONENT(Transform)
  private:
    Vector3f _position;
    Quatf _rotation;
  public:
    inline Transform() : _position(0, 0, 0) {}

    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    inline Vector3f toAbsolute(const Vector3f& v) const { return position()+_rotation.rotate(v); }
    inline Vector3f fromAbsolute(const Vector3f& v) const { return _rotation.inverse().rotate(v-position()); }
    inline void rotate(const Quatf& q) { _rotation = _rotation * q; }
    inline void rotate(const Vector3f& v, float d) { rotate(Quatf(v, d)); }
    inline void rotate_absolute(const Quatf& q) { _rotation = q * _rotation; }
    inline void rotate_absolute(const Vector3f& v, float d) { rotate_absolute(Quatf(v, d)); }
    inline void phiLook(float d) { rotate(Vector3f(1, 0, 0), d); }
    inline void thetaLook(float d) { rotate(Vector3f(0, 0, 1), d); }
    inline void move(const Vector3f& d) { _position += _rotation.rotate(d); }
    inline void move_absolute(const Vector3f& v) { _position += v; }

    inline void position(const Vector3f& p) { _position = p; }
    inline const Vector3f& position() const { return _position; }
    inline void rotation(const Quatf& r) { _rotation = r; }
    inline void rotation(const Vector3f& axis, float angle) { _rotation = Quatf(axis, angle); }
    inline const Quatf& rotation() const { return _rotation; }
    inline Vector3f right() const { return rotation().rotate(Vector3f(1, 0, 0)); }
    inline Vector3f forward() const { return rotation().rotate(Vector3f(0, 1, 0)); }
    inline Vector3f up() const { return rotation().rotate(Vector3f(0, 0, 1)); }
    inline Matrix44f matrix() const { return SQTToMat(rotation(), position()); }
  };
}
