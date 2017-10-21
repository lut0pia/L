#include "StaticMesh.h"

#include "../gl/GL.h"
#include "../engine/SharedUniform.h"

using namespace L;

Map<Symbol, Var> StaticMesh::pack() const {
  Map<Symbol, Var> data;
  data["mesh"] = _mesh;
  data["material"] = _material;
  data["scale"] = _scale;
  return data;
}
void StaticMesh::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "mesh", _mesh);
  unpack_item(data, "material", _material);
  unpack_item(data, "scale", _scale);
}

void StaticMesh::render(const Camera& c) {
  if(_mesh && _material && _material->valid()) {
    _material->use(SQTToMat(_transform->rotation(), _transform->position(), _scale));
    _mesh->draw();
  }
}
