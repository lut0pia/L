#pragma once

#include "Component.h"
#include "Transform.h"
#include "../rendering/Material.h"
#include "../engine/CullVolume.h"
#include "../engine/Resource.h"
#include "../rendering/Mesh.h"

namespace L {
  class MeshComponent : public Component {
    L_COMPONENT(MeshComponent)
      L_COMPONENT_HAS_RENDER(MeshComponent)
      L_COMPONENT_HAS_LATE_UPDATE(MeshComponent)
  private:
    Transform* _transform;
    CullVolume _cull_volume;
    Resource<Mesh> _mesh;
    Material _material;
    float _scale;
  public:
    inline MeshComponent() : _scale(1.f) {}

    inline void update_components() { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    void late_update();
    void render(const Camera&);

    inline void mesh(const char* filename) { _mesh = Resource<Mesh>::get(filename); }
    inline void material(const char* filename) { _material.parent(Resource<Material>::get(filename)); }
    Material* material() { return &_material; }
    inline void scale(float scale) { _scale = scale; }
  };
}
