#include "Camera.h"

#include "../constants.h"
#include "../system/Window.h"

using namespace L;

Camera::Camera() : _viewport(Vector2f(0,0),Vector2f(1,1)) {}
void Camera::start() {
  _transform = entity().component<Transform>();
}
void Camera::prerender() {
  static Matrix44f camOrient(Matrix44f::orientation(Vector3f(1,0,0),Vector3f(0,0,1),Vector3f(0,-1,0)).inverse());
  Matrix44f orientation(Matrix44f::orientation(_transform->right(),_transform->forward(),_transform->up()));
  _view = camOrient * _transform->absolute().inverse();
  _viewProjection = _projection*_view;
  _ray = orientation*_projection.inverse();
  glLoadTransposeMatrixf(_viewProjection.array());
  glViewport(_viewport.min().x()*Window::width(),_viewport.min().y()*Window::height(),
             _viewport.size().x()*Window::width(),_viewport.size().y()*Window::height());
}

void Camera::perspective(float fovy, float aspect, float near, float far) {
  _projection = Matrix44f::identity();
  float top(near*tan(fovy*(PI<float>()/360.f))), right(top*aspect);
  _projection(0,0) = near/right;
  _projection(1,1) = near/top;
  _projection(2,2) = (-far+near)/(far-near);
  _projection(2,3) = (-2.f*far*near)/(far-near);
  _projection(3,2) = -1.f;
  _projection(3,3) = 0.f;
  update();
}
void Camera::ortho(float left, float right, float bottom, float top, float near, float far) {
  _projection = Matrix44f::identity();
  _projection(0,0) = 2.f/(right-left);
  _projection(1,1) = 2.f/(top-bottom);
  _projection(2,2) = -2.f/(far-near);
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
