#pragma once

#include "Transform.h"

namespace L {
  class HierarchyComponent : public Component {
    L_COMPONENT(HierarchyComponent)
      L_COMPONENT_HAS_LATE_UPDATE(HierarchyComponent)
  protected:
    Transform *_parent, *_transform;
    Vector3f _translation;
    Quatf _rotation;
  public:
    inline HierarchyComponent() : _parent(nullptr), _translation(0.f) {}

    void late_update();
    void update_components();
    static void script_registration();

    inline void parent(Transform* t) { _parent = t; }
    inline void translation(const Vector3f& t) { _translation = t; }
    inline void rotation(const Quatf& r) { _rotation = r; }
    inline void rotation(const Vector3f& axis, float angle) { _rotation = Quatf(axis, angle);; }

    inline void rotate(const Quatf& q) { _rotation = _rotation * q; }
    inline void rotate(const Vector3f& v, float d) { rotate(Quatf(v, d)); }
    inline void rotate_absolute(const Quatf& q) { _rotation = q * _rotation; }
    inline void rotate_absolute(const Vector3f& v, float d) { rotate_absolute(Quatf(v, d)); }
  };
}
