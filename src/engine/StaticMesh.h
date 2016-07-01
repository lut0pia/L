#pragma once

#include "Component.h"
#include "../gl/Mesh.h"

namespace L {
  class StaticMesh : public Component {
      L_COMPONENT(StaticMesh)
    private:
      Transform* _transform;
      Ref<GL::Mesh> _mesh;
    public:
      inline void start() {_transform = entity()->requireComponent<Transform>();}
      inline void mesh(const char* filename) {_mesh = Engine::mesh(filename);}
      inline void render(const Camera&) {_mesh->draw();}
  };
}
