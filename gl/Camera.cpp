#include "Camera.h"

#include <GL/glew.h>
#include <GL/glu.h>

using namespace L;
using namespace GL;

Camera::Camera(const Point3f& position, float fovy, float aspect, float nearLimit, float farLimit)
  : _position(position), _lookat(position+Point3f(0,0,-1)), _up(0,1,0), _fovy(fovy), _aspect(aspect), _nearLimit(nearLimit), _farLimit(farLimit) {
  update();
}


void Camera::move(const Point3f& delta) {
  Point3f absDelta(_right*delta.x()+_up*delta.y()+_forward*delta.z());
  _position += absDelta;
  _lookat += absDelta;
  update();
}
void Camera::position(const Point3f& position) {
  _position = position;
  update();
}
void Camera::lookat(const Point3f& lookat) {
  _lookat = lookat;
  update();
}

void Camera::phi(float angle) {
  _lookat -= _position;
  _lookat = Matrix33f::rotation(Point3f(0,1,0),angle) * _lookat;
  _lookat += _position;
  update();
}

void Camera::theta(float angle) {
  _lookat -= _position;
  _lookat = Matrix33f::rotation(_right,angle) * _lookat;
  _lookat += _position;
}

void Camera::fovy(float f) {
  _fovy = f;
  update();
}
void Camera::aspect(float a) {
  _aspect = a;
  update();
}
void Camera::clipping(float n, float f) {
  _nearLimit = n;
  _farLimit = f;
  update();
}

void Camera::update() {
  // Communicate perspective information to OpenGL
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(_fovy,_aspect,_nearLimit,_farLimit);
  // Compute new vectors
  _forward = _lookat - _position;
  _forward.normalize();
  _right = _forward.cross(Point3f(0,1,0));
  _right.normalize();
  _up = _right.cross(_forward);
  _up.normalize();
  // Compute new view matrix
  _view = Matrix44f::orientation(_right,_up,-_forward).transpose() * Matrix44f::translation(-_position);
}
void Camera::place() {
  glMatrixMode(GL_MODELVIEW);
  //gluLookAt(_position.x(), _position.y(), _position.z(), _lookat.x(), _lookat.y(), _lookat.z(), _up.x(), _up.y(), _up.z());
  //glLoadTransposeMatrixf(_view.array());
  glLoadIdentity();
}

