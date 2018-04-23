#include "MeshComponent.h"

#include "../rendering/GL.h"
#include "../rendering/shader_lib.h"

using namespace L;

Map<Symbol, Var> MeshComponent::pack() const {
  Map<Symbol, Var> data;
  data["mesh"] = _mesh;
  data["material"] = _material;
  data["scale"] = _scale;
  return data;
}
void MeshComponent::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "mesh", _mesh);
  unpack_item(data, "material", _material);
  unpack_item(data, "scale", _scale);
}
void MeshComponent::script_registration() {
  L_COMPONENT_BIND(MeshComponent, "mesh");
  L_COMPONENT_METHOD(MeshComponent, "mesh", 1, mesh((const char*)c.local(0).get<String>()));
  L_COMPONENT_RETURN_METHOD(MeshComponent, "material", 0, material());
  L_COMPONENT_METHOD(MeshComponent, "scale", 1, scale(c.local(0).get<float>()));
}

void MeshComponent::render(const Camera& c) {
  if(_mesh && _material.valid()) {
    _material.use(SQTToMat(_transform->rotation(), _transform->position(), _scale));
    _mesh->draw();
  }
}
