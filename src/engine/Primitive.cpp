#include "Primitive.h"

#include "../gl/GL.h"

using namespace L;

void Primitive::updateComponents(){
  _transform = entity()->requireComponent<Transform>();
}
void Primitive::render(const Camera& camera){
  GL::baseProgram().use();
  GL::baseProgram().uniform("model",_transform->matrix()*Matrix44f::scale(_radius));
  switch(_type){
    case Box:
      GL::cube().draw();
      break;
    case Sphere:
      GL::sphere().draw();
      break;
  }
}