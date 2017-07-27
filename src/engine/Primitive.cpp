#include "Primitive.h"

#include "../gl/GL.h"

using namespace L;

void Primitive::updateComponents(){
  _transform = entity()->requireComponent<Transform>();
}
Map<Symbol, Var> Primitive::pack() const {
  Map<Symbol, Var> data;
  data["type"] = Symbol(_type==Box ? "box" : "sphere");
  data["center"] = _center;
  data["radius"] = _radius;
  data["color"] = _color;
  return data;
}
void Primitive::unpack(const Map<Symbol, Var>& data) {
  {
    Symbol type;
    unpack_item(data, "type", type);
    if(type==Symbol("sphere"))
      _type = Sphere;
    else if(type==Symbol("box"))
      _type = Box;
  }
  unpack_item(data, "center", _center);
  unpack_item(data, "radius", _radius);
  unpack_item(data, "color", _color);
}

void Primitive::render(const Camera& camera){
  GL::baseColorProgram().use();
  GL::baseColorProgram().uniform("model",_transform->matrix()*scale_matrix(_radius));
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