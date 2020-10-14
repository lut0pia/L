#include "Transform.h"

using namespace L;

void Transform::script_registration() {
  L_COMPONENT_BIND(Transform, "transform");
  L_SCRIPT_RETURN_METHOD(Transform, "get_position", 0, position());
  L_SCRIPT_RETURN_METHOD(Transform, "right", 0, right());
  L_SCRIPT_RETURN_METHOD(Transform, "forward", 0, forward());
  L_SCRIPT_RETURN_METHOD(Transform, "up", 0, up());
  L_SCRIPT_RETURN_METHOD(Transform, "to_absolute", 1, to_absolute(c.param(0)));
  L_SCRIPT_METHOD(Transform, "set_position", 1, position(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(Transform, "set_rotation", 1, rotation(c.param(0).get<Quatf>()));
  L_SCRIPT_METHOD(Transform, "move", 1, move(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(Transform, "move_absolute", 1, move_absolute(c.param(0).get<Vector3f>()));
  L_SCRIPT_METHOD(Transform, "rotate", 1, rotate(c.param(0).get<Quatf>()));
  L_SCRIPT_METHOD(Transform, "rotate_absolute", 1, rotate_absolute(c.param(0).get<Quatf>()));
}
