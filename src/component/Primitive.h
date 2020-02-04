#pragma once

#include "Transform.h"
#include "../engine/CullVolume.h"
#include "../rendering/Material.h"
#include "../engine/Resource.h"

namespace L {
  class Primitive : public TComponent<Primitive,
    ComponentFlag::Render> {
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
    
    void render(const Camera& camera, const RenderPass& render_pass);

    inline void material(const char* filename) { _material.parent(filename); }
    inline Material* material() { return &_material; }
    inline void scale(const Vector3f& s) { _scale = s; }

    static void custom_late_update_all();
  };
}
