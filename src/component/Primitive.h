#pragma once

#include "Transform.h"
#include "../engine/CullVolume.h"
#include "../rendering/Material.h"
#include "../engine/Resource.h"

namespace L {
  class Primitive : public Component {
    L_COMPONENT(Primitive)
      L_COMPONENT_HAS_RENDER(Primitive)
  protected:
    Transform* _transform;
    CullVolume _cull_volume;
    Interval3f _bounds;
    Material _material;
    Vector3f _scale = 1.f;
  public:
    virtual void update_components() override;
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();
    
    void render(const Camera& camera);

    inline void material(const char* filename) { _material.parent(Resource<Material>::get(filename)); }
    inline Material* material() { return &_material; }
    inline void scale(const Vector3f& s) { _scale = s; }

    static void late_update_all();
  };
}
