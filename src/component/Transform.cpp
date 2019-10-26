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
  L_SCRIPT_RETURN_METHOD(Transform, "get_position", 0, position());
  L_SCRIPT_RETURN_METHOD(Transform, "right", 0, right());
  L_SCRIPT_RETURN_METHOD(Transform, "forward", 0, forward());
  L_SCRIPT_RETURN_METHOD(Transform, "up", 0, up());
  L_SCRIPT_RETURN_METHOD(Transform, "to_absolute", 1, to_absolute(c.param(0)));
  L_SCRIPT_METHOD(Transform, "set_position", 1, position(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(Transform, "set_rotation", 2, rotation(c.param(0).get<Vector3f>(), c.param(1).get<float>()));
  L_SCRIPT_METHOD(Transform, "move", 1, move(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(Transform, "move_absolute", 1, move_absolute(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(Transform, "rotate", 2, rotate(c.param(0).get<Vector3f>(), c.param(1).get<float>()));
  L_SCRIPT_METHOD(Transform, "rotate_absolute", 2, rotate_absolute(c.param(0).get<Vector3f>(), c.param(1).get<float>()));
}
