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
