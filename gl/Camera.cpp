#include "Camera.h"

#include <GL/glew.h>
#include <GL/glu.h>

using namespace L;
using namespace GL;

Camera::Camera(const Point3f& position, float fovy, float aspect, float nearLimit, float farLimit)
  : _position(position), _lookat(position+Point3f(0,1,0)), _up(0,0,1), _fovy(fovy), _aspect(aspect), _nearLimit(nearLimit), _farLimit(farLimit) {
  update();
}


void Camera::move(const Point3f& delta) {
  Point3f absDelta(_right*delta.x()+_forward*delta.y()+_up*delta.z());
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
  _lookat = Matrix33f::rotation(Point3f(0,0,1),angle) * _lookat;
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
  _right = _forward.cross(Point3f(0,0,1));
  _right.normalize();
  _up = _right.cross(_forward);
  _up.normalize();
  // Compute new view matrix
  _view = Matrix44f::identity();
  // R2 = _forward
  /*
  NYVert3Df R0(up.vecProd(R2));
  R0.normalize();
  NYVert3Df R1(R2.vecProd(R0));
  NYVert3Df neg(-pos.X,-pos.Y,-pos.Z);
  float D0 = R0.scalProd(neg);
  float D1 = R1.scalProd(neg);
  float D2 = R2.scalProd(neg);
  Mat.Direct._11 = R0.X;
  Mat.Direct._12 = R0.Y;
  Mat.Direct._13 = R0.Z;
  Mat.Direct._21 = R1.X;
  Mat.Direct._22 = R1.Y;
  Mat.Direct._23 = R1.Z;
  Mat.Direct._31 = R2.X;
  Mat.Direct._32 = R2.Y;
  Mat.Direct._33 = R2.Z;
  Mat.Direct._14 = D0;
  Mat.Direct._24 = D1;
  Mat.Direct._34 = D2;
  */
}
void Camera::place() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  /*
  glRotatef(-prs->rotation.x()-90,1,0,0);
  glRotatef(-prs->rotation.y(),0,1,0);
  glRotatef(-prs->rotation.z(),0,0,1);
  glTranslatef(-prs->position.x(),-prs->position.y(),-prs->position.z());
  */
  gluLookAt(_position.x(), _position.y(), _position.z(), _lookat.x(), _lookat.y(), _lookat.z(), _up.x(), _up.y(), _up.z());
}

