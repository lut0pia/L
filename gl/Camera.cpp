#include "Camera.h"

#include <GL/glew.h>
#include <GL/glu.h>

using namespace L;
using namespace GL;

Camera::Camera(const Point3f& position)
  : _position(position), _lookat(position+Point3f(0,0,-1)), _up(0,1,0) {
  perspective(60,16.0/9.0,.01,512);
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
  update();
}

void Camera::perspective(float fovy, float aspect, float near, float far) {
  _fovy = fovy;
  _aspect = aspect;
  _near = near;
  _far = far;
  _projection = Matrix44f::identity();
  float top(near*tan(fovy*(M_PI/360))), right(top*aspect);
  _projection(0,0) = near/right;
  _projection(1,1) = near/top;
  _projection(2,2) = (-far+near)/(far-near);
  _projection(2,3) = (-2*far*near)/(far-near);
  _projection(3,2) = -1;
  _projection(3,3) = 0;
}
void Camera::ortho(float left, float right, float bottom, float top, float near, float far) {
}
void Camera::pixels() {
  //ortho(0,Window::width(),0,Window::height());
}
Point3f Camera::screenToRay(const Point2f& p) const {
  float fovx(_aspect*_fovy);
  return Matrix33f::rotation(_up,-fovx*(M_PI/360)*p.x()) * Matrix33f::rotation(_right,_fovy*(M_PI/360)*p.y()) * _forward;
}

void Camera::update() {
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

