#pragma once

#include "Transform.h"

namespace L {
  class HierarchyComponent : public TComponent<HierarchyComponent,
    ComponentFlag::LateUpdate> {
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
    inline void translate(const Vector3f& t) { _translation += t; }

    inline void rotation(const Quatf& r) { _rotation = r; }
    inline void rotate(const Quatf& q) { _rotation = _rotation * q; }
    inline void rotate_absolute(const Quatf& q) { _rotation = q * _rotation; }
  };
}
