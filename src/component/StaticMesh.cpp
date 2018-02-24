#include "StaticMesh.h"

#include "../rendering/GL.h"
#include "../rendering/shader_lib.h"

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
void StaticMesh::script_registration() {
  L_COMPONENT_BIND(StaticMesh, "staticmesh");
  L_COMPONENT_METHOD(StaticMesh, "mesh", 1, mesh((const char*)c.local(0).get<String>()));
  L_COMPONENT_METHOD(StaticMesh, "material", 1, material((const char*)c.local(0).get<String>()));
  L_COMPONENT_METHOD(StaticMesh, "scale", 1, scale(c.local(0).get<float>()));
}

void StaticMesh::render(const Camera& c) {
  if(_mesh && _material && _material->valid()) {
    _material->use(SQTToMat(_transform->rotation(), _transform->position(), _scale));
    _mesh->draw();
  }
}
