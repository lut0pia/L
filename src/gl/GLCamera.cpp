#include "GLCamera.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include "../constants.h"
#include "../system/Window.h"

using namespace L;
using namespace GL;

Camera::Camera(const Vector3f& position,const Vector3f& lookat)
  : _position(position), _lookat(lookat) {
  update();
}
void Camera::update() {
  // Compute new vectors
  _forward = _lookat - _position;
  _forward.normalize();
  _right = _forward.cross(Vector3f(0,1,0));
  _right.normalize();
  _up = _right.cross(_forward);
  _up.normalize();
  // Compute new view matrix
  Matrix44f orientation(Matrix44f::orientation(_right,_up,-_forward));
  _view = orientation.transpose() * Matrix44f::translation(-_position);
  _viewProjection = _projection*_view;
  _ray = orientation*_projection.inverse();
}

void Camera::move(const Vector3f& delta) {
  Vector3f absDelta(_right*delta.x()+_up*delta.y()+_forward*delta.z());
  _position += absDelta;
  _lookat += absDelta;
  update();
}
void Camera::position(const Vector3f& position) {
  _position = position;
  update();
}
void Camera::lookat(const Vector3f& lookat) {
  _lookat = lookat;
  update();
}

void Camera::phiLook(float angle) {
  _lookat -= _position;
  _lookat = Vector3f(Matrix44f::rotation(Vector3f(0,1,0),angle) * _lookat);
  _lookat += _position;
  update();
}
void Camera::phiPosition(float angle) {
  _position -= _lookat;
  _position = Vector3f(Matrix44f::rotation(Vector3f(0,1,0),angle) * _position);
  _position += _lookat;
  update();
}
void Camera::thetaLook(float angle) {
  _lookat -= _position;
  _lookat = Vector3f(Matrix44f::rotation(_right,angle) * _lookat);
  _lookat += _position;
  update();
}
void Camera::thetaPosition(float angle) {
  _position -= _lookat;
  _position = Vector3f(Matrix44f::rotation(_right,angle) * _position);
  _position += _lookat;
  update();
}

void Camera::perspective(float fovy, float aspect, float near, float far) {
  _projection = Matrix44f::identity();
  float top(near*tan(fovy*(PI<float>()/360))), right(top*aspect);
  _projection(0,0) = near/right;
  _projection(1,1) = near/top;
  _projection(2,2) = (-far+near)/(far-near);
  _projection(2,3) = (-2*far*near)/(far-near);
  _projection(3,2) = -1;
  _projection(3,3) = 0;
  update();
}
void Camera::ortho(float left, float right, float bottom, float top, float near, float far) {
  _projection = Matrix44f::identity();
  _projection(0,0) = 2/(right-left);
  _projection(1,1) = 2/(top-bottom);
  _projection(2,2) = -2/(far-near);
  _projection(0,3) = -(right+left)/(right-left);
  _projection(1,3) = -(top+bottom)/(top-bottom);
  _projection(2,3) = -(far+near)/(far-near);
  update();
}
void Camera::pixels() {
  ortho(0,Window::width(),Window::height(),0);
}

bool Camera::worldToScreen(const Vector3f& p, Vector2f& wtr) const {
  Vector4f q(_viewProjection*p);
  if(q.w()>0) // We do not want values behind the camera
    wtr = Vector3f(q)/q.w();
  else return false;
  return true;
}
Vector3f Camera::screenToRay(const Vector2f& p) const {
  return Vector3f(_ray * Vector4f(p.x(),p.y(),0,1));
}
bool Camera::sees(const Interval3f& i) const {
  static const Interval3f ndc(Vector3f(-1,-1,-1),Vector3f(1,1,1));
  Interval3f projected;
  for(int c(0); c<8; c++) { // Cycle through corners of the interval
    Vector3f p(((c&0x1)?i.min().x():i.max().x()),
              ((c&0x2)?i.min().y():i.max().y()),
              ((c&0x4)?i.min().z():i.max().z()));
    Vector4f q(_viewProjection*p);
    if(q.w()>0) { // We do not want values behind the camera
      Vector3f r(Vector3f(q)/q.w()); // Compute NDC coordinates
      if(abs(r.x())<1 && abs(r.y())<1 && abs(r.z())<1) // The point is clearly in view
        return true;
      else { // The shape could go through the view
        projected.add(r);
        if(ndc.overlaps(projected))
          return true;
      }
    }
  }
  return false;
}

