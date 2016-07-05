#pragma once

#include "Component.h"
#include "Transform.h"
#include "../gl/Mesh.h"

namespace L {
  class StaticMesh : public Component {
    L_COMPONENT(StaticMesh)
  private:
    Transform* _transform;
    Ref<GL::Mesh> _mesh;
    Ref<GL::Texture> _texture;
  public:
    inline void start() { _transform = entity()->requireComponent<Transform>(); }
    inline void mesh(const char* filename) { _mesh = Engine::mesh(filename); }
    inline void texture(const char* filename) { _texture = Engine::texture(filename); }
    void render(const Camera&);
  };
}
