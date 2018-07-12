#include "Primitive.h"

#include "../component/Camera.h"

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

void Primitive::render(const Camera& camera) {
  if(_cull_volume.visible() && _material.valid()) {
    _material.draw(camera.cmd_buffer(), _transform->matrix()*scale_matrix(_scale));
  }
}

void Primitive::late_update_all() {
  {
    L_SCOPE_MARKER("Primitive bounds computation");
    ComponentPool<Primitive>::async_iterate([](Primitive& c, uint32_t) {
      const Interval3f model_bounds(c._material.bounds());
      c._bounds = c._transform->position();
      for(uint32_t i(0); i<8; i++)
        c._bounds.add(c._transform->toAbsolute(Vector3f(
          (i&1 ? model_bounds.min().x() : model_bounds.max().x())*c._scale.x(),
          (i&2 ? model_bounds.min().y() : model_bounds.max().y())*c._scale.y(),
          (i&4 ? model_bounds.min().z() : model_bounds.max().z())*c._scale.z()
        )));
    });
  }
  {
    L_SCOPE_MARKER("Primitive cull volume update");
    ComponentPool<Primitive>::iterate([](Primitive& c) {
      c._cull_volume.update_bounds(c._bounds);
    });
  }
}
