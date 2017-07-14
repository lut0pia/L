#pragma once

#include "Component.h"
#include "Transform.h"
#include "Resource.h"
#include "../gl/Mesh.h"

namespace L {
  class StaticMesh : public Component {
    L_COMPONENT(StaticMesh)
  private:
    Transform* _transform;
    String _mesh_path, _texture_path;
    Ref<GL::Mesh> _mesh;
    Ref<GL::Texture> _texture;
    float _scale;
  public:
    inline StaticMesh() : _scale(1.f) {}

    inline void updateComponents() { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    inline void mesh(const char* filename) { _mesh_path = filename; _mesh = Resource::mesh(filename); }
    inline void texture(const char* filename) { _texture_path = filename; _texture = Resource::texture(filename); }
    inline void scale(float scale) { _scale = scale; }
    void render(const Camera&);
  };
}
