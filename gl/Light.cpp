#include "Light.h"

using namespace L;
using namespace GL;


Light::Light() : _position {0,0,0,1}, _color {1,1,1,1} {
}
void Light::setPosition(float x, float y, float z, float w) {
  _position[0] = x;
  _position[1] = y;
  _position[2] = z;
  _position[3] = w;
}
void Light::setColor(float r, float g, float b, float a) {
  _color[0] = r;
  _color[1] = g;
  _color[2] = b;
  _color[3] = a;
}
void Light::set(GLenum light) {
  glLightfv(light, GL_POSITION, _position);
  glLightfv(light, GL_DIFFUSE, _color);
}
