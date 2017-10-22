#pragma once

#include "Transform.h"
#include "../engine/Material.h"
#include "../engine/Resource.h"

namespace L {
  class Primitive : public Component {
    L_COMPONENT(Primitive)
      L_COMPONENT_HAS_RENDER(Primitive)
  protected:
    Transform* _transform;
    Material _material;
    Vector3f _scale = 1.f;
  public:
    virtual void update_components() override;
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    inline void material(const char* filename) { _material.parent(Resource<Material>::get(filename)); }
    Material* material() { return &_material; }
    inline void scale(const Vector3f& s) { _scale = s; }
    void render(const Camera& camera);
  };
}
