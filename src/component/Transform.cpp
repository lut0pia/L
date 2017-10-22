#include "Transform.h"

using namespace L;

Map<Symbol, Var> Transform::pack() const {
  Map<Symbol, Var> data;
  data["position"] = _position;
  data["rotation"] = _rotation;
  return data;
}
void Transform::unpack(const Map<Symbol, Var>& data) {
  unpack_item(data, "position", _position);
  unpack_item(data, "rotation", _rotation);
}
void Transform::script_registration(){
  L_COMPONENT_BIND(Transform, "transform");
  L_COMPONENT_RETURN_METHOD(Transform, "get-position", 0, position());
  L_COMPONENT_RETURN_METHOD(Transform, "right", 0, right());
  L_COMPONENT_RETURN_METHOD(Transform, "forward", 0, forward());
  L_COMPONENT_RETURN_METHOD(Transform, "up", 0, up());
  L_COMPONENT_METHOD(Transform, "set-position", 1, position(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Transform, "set-rotation", 2, rotation(c.local(0).get<Vector3f>(), c.local(1).get<float>()));
  L_COMPONENT_METHOD(Transform, "move", 1, move(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Transform, "move-absolute", 1, move_absolute(c.local(0).get<Vector3f>()));
  L_COMPONENT_METHOD(Transform, "rotate", 2, rotate(c.local(0).get<Vector3f>(), c.local(1).get<float>()));
  L_COMPONENT_METHOD(Transform, "rotate-absolute", 2, rotate_absolute(c.local(0).get<Vector3f>(), c.local(1).get<float>()));
}
