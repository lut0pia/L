#include "Primitive.h"

#include "../gl/GL.h"

using namespace L;

void Primitive::updateComponents(){
  _transform = entity()->requireComponent<Transform>();
}
void Primitive::render(const Camera& camera){
  GL::baseColorProgram().use();
  GL::baseColorProgram().uniform("model",_transform->matrix()*Matrix44f::scale(_radius));
  GL::baseColorProgram().uniform("color",_color);
  switch(_type){
    case Box:
      GL::cube().draw();
      break;
    case Sphere:
      GL::sphere().draw();
      break;
  }
}