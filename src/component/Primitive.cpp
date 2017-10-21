#include "Primitive.h"

#include "../gl/GL.h"

using namespace L;

void Primitive::updateComponents() {
  _transform = entity()->requireComponent<Transform>();
}
Map<Symbol, Var> Primitive::pack() const {
  return Map<Symbol, Var> {
    {"material", _material},
    {"scale", _scale},
  };
}
void Primitive::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "material", _material);
  unpack_item(data, "scale", _scale);
}

void Primitive::render(const Camera& camera) {
  if(_material.drawable()) {
    _material.use(_transform->matrix()*scale_matrix(_scale));
    GL::draw(_material.final_primitive_mode(), _material.final_vertex_count());
  }
}
