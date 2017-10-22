#pragma once

#include "Component.h"
#include "Transform.h"
#include "../engine/Material.h"
#include "../engine/Resource.h"
#include "../gl/Mesh.h"

namespace L {
  class StaticMesh : public Component {
    L_COMPONENT(StaticMesh)
      L_COMPONENT_HAS_RENDER(StaticMesh)
  private:
    Transform* _transform;
    Resource<GL::Mesh> _mesh;
    Resource<Material> _material;
    float _scale;
  public:
    inline StaticMesh() : _scale(1.f) {}

    inline void update_components() { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    inline void mesh(const char* filename) { _mesh = Resource<GL::Mesh>::get(filename); }
    inline void material(const char* filename) { _material = Resource<Material>::get(filename); }
    inline void scale(float scale) { _scale = scale; }
    void render(const Camera&);
  };
}
