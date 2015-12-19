#include "Transform.h"

using namespace L;


Vector3f Transform::right() {
  return Vector3f(absolute()(0,0),absolute()(1,0),absolute()(2,0));
}
Vector3f Transform::forward() {
  return Vector3f(absolute()(0,1),absolute()(1,1),absolute()(2,1));
}
Vector3f Transform::up() {
  return Vector3f(absolute()(0,2),absolute()(1,2),absolute()(2,2));
}
Vector3f Transform::position() {
  return Vector3f(absolute()(0,3),absolute()(1,3),absolute()(2,3));
}
const Matrix44f& Transform::absolute() {
  if(_parent) {
    if(Engine::frame()!=_absoluteFrame) {
      _absolute = _parent->absolute()*_relative;
      _absoluteFrame = Engine::frame();
    }
    return _absolute;
  } else return _relative;
}
