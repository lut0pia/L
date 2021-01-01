#include "Primitive.h"

#include "../component/Camera.h"
#include "../engine/Engine.h"
#include "../engine/Resource.inl"

using namespace L;

void Primitive::update_components() {
  _transform = entity()->require_component<Transform>();
}
void Primitive::script_registration() {
  L_COMPONENT_BIND(Primitive, "primitive");
  L_SCRIPT_RETURN_METHOD(Primitive, "material", 0, material().handle());
  L_SCRIPT_METHOD(Primitive, "scale", 1, scale(c.param(0).get<Vector3f>()));

  Engine::add_late_update(custom_late_update_all);
}

void Primitive::render(const Camera& camera, const RenderPassImpl* render_pass) {
  if(_cull_volume.visible() && _material.valid_for_render_pass(render_pass)) {
    _material.draw(camera, render_pass, _transform->matrix()*scale_matrix(_scale));
  }
}

void Primitive::custom_late_update_all() {
  {
    L_SCOPE_MARKER("Primitive material update");
    ComponentPool<Primitive>::iterate([](Primitive& c) {
      c._material.update();
    });
  }
  {
    L_SCOPE_MARKER("Primitive bounds computation");
    ComponentPool<Primitive>::async_iterate([](Primitive& c, uint32_t) {
      const Interval3f model_bounds(c._material.bounds());
      c._bounds = c._transform->position();
      for(uint32_t i(0); i<8; i++)
        c._bounds.add(c._transform->to_absolute(Vector3f(
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
