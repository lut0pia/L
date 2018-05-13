#include "Primitive.h"

#include "../rendering/GL.h"

using namespace L;

void Primitive::update_components() {
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
void Primitive::script_registration() {
  L_COMPONENT_BIND(Primitive, "primitive");
  L_COMPONENT_RETURN_METHOD(Primitive, "material", 0, material());
  L_COMPONENT_METHOD(Primitive, "scale", 1, scale(c.local(0).get<Vector3f>()));
}

void Primitive::late_update() {
  const Interval3f model_bounds(_material.bounds());
  Interval3f world_bounds(_transform->position());
  for(uint32_t i(0); i<8; i++)
    world_bounds.add(_transform->toAbsolute(Vector3f(
      (i&1 ? model_bounds.min().x() : model_bounds.max().x())*_scale.x(),
      (i&2 ? model_bounds.min().y() : model_bounds.max().y())*_scale.y(),
      (i&4 ? model_bounds.min().z() : model_bounds.max().z())*_scale.z()
    )));
  _cull_volume.update_bounds(world_bounds);
}
void Primitive::render(const Camera& camera) {
  if(_cull_volume.visible() && _material.valid()) {
    _material.draw(_transform->matrix()*scale_matrix(_scale));
  }
}
