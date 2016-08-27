#include "Camera.h"

#include "../constants.h"
#include "../gl/Buffer.h"
#include "../gl/GL.h"
#include "../gl/Program.h"
#include "../system/Window.h"
#include "SharedUniform.h"

using namespace L;

static const GLuint attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

Camera::Camera() :
  _viewport(Vector2f(0,0),Vector2f(1,1)),
  _gcolor(0,GL_RGBA,Window::width(),Window::height(),0,GL_RGBA,GL_UNSIGNED_BYTE),
  _gnormal(0,GL_RGB16F,Window::width(),Window::height(),0,GL_RGB,GL_FLOAT),
  _gdepth(0,GL_DEPTH_COMPONENT24,Window::width(),Window::height(),0,GL_DEPTH_COMPONENT,GL_FLOAT),
  _gbuffer(GL_FRAMEBUFFER,{&_gcolor,&_gnormal},&_gdepth,attachments,2){
  _gcolor.parameter(GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  _gcolor.parameter(GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  _gnormal.parameter(GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  _gnormal.parameter(GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  _gdepth.parameter(GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  _gdepth.parameter(GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}
void Camera::updateComponents() {
  _transform = entity()->requireComponent<Transform>();
}
void Camera::prerender() {
  static Matrix44f camOrient(Matrix44f::orientation(Vector3f(1,0,0),Vector3f(0,0,1),Vector3f(0,-1,0)).inverse());
  Matrix44f orientation(Matrix44f::orientation(_transform->right(),_transform->forward(),_transform->up()));
  _view = camOrient * _transform->matrix().inverse();
  _prevViewProjection = _viewProjection;
  _viewProjection = _projection*_view;
  _ray = orientation*_projection.inverse();
  glViewport(_viewport.min().x()*Window::width(),_viewport.min().y()*Window::height(),
             _viewport.size().x()*Window::width(),_viewport.size().y()*Window::height());
  Engine::sharedUniform().subData(L_SHAREDUNIFORM_VIEW,sizeof(Matrix44f),_view.array());
  Engine::sharedUniform().subData(L_SHAREDUNIFORM_INVVIEW,sizeof(Matrix44f),_view.inverse().array());
  Engine::sharedUniform().subData(L_SHAREDUNIFORM_VIEWPROJ,sizeof(Matrix44f),_viewProjection.array());
  Engine::sharedUniform().subData(L_SHAREDUNIFORM_INVVIEWPROJ,sizeof(Matrix44f),_viewProjection.inverse().array());
  Engine::sharedUniform().subData(L_SHAREDUNIFORM_PREVVIEWPROJ,sizeof(Matrix44f),_prevViewProjection.array());
  Engine::sharedUniform().subData(L_SHAREDUNIFORM_EYE,sizeof(Vector3f),_transform->absolutePosition().array());
  _gbuffer.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Camera::postrender(){
  static GL::Program deferredProgram(GL::Shader(
    "#version 330 core\n"
    "layout (location = 0) in vec3 vertex;"
    "out vec2 ftexcoords;"
    "void main(){"
    "ftexcoords = (vertex.xy+1.f)*.5f;"
    "gl_Position = vec4(vertex,1.f);"
    "}",GL_VERTEX_SHADER),
    GL::Shader(
      "#version 330 core\n"
      L_SHAREDUNIFORM
      "in vec2 ftexcoords;"
      "out vec4 fragcolor;"
      "uniform sampler2D colorBuffer;"
      "uniform sampler2D normalBuffer;"
      "uniform sampler2D depthBuffer;"
      "void main(){"
      "vec3 color = texture(colorBuffer,ftexcoords).rgb;"
      "vec3 normal = decodeNormal(texture(normalBuffer,ftexcoords).xy);"
      "float depth = texture(depthBuffer,ftexcoords).r;"
      "vec4 position = invViewProj * vec4(ftexcoords*2.f-1.f,depth*2.f-1.f,1.f);"
      "position = vec4(position.xyz/position.w,1.f);"
      "fragcolor = vec4(color,1.f) * (1.f+dot(normal,normalize(vec3(1,-2,3))))*.5f;"
      "}",GL_FRAGMENT_SHADER));
  _gbuffer.unbind();
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  deferredProgram.use();
  deferredProgram.uniform("colorBuffer",_gcolor,GL_TEXTURE0);
  deferredProgram.uniform("normalBuffer",_gnormal,GL_TEXTURE1);
  deferredProgram.uniform("depthBuffer",_gdepth,GL_TEXTURE2);
  GL::quad().draw();
  Window::swapBuffers();
}

void Camera::perspective(float fovy,float aspect,float near,float far) {
  _projection = Matrix44f(1.f);
  float top(near*tan(fovy*(PI<float>()/360.f))),right(top*aspect);
  _projection(0,0) = near/right;
  _projection(1,1) = near/top;
  _projection(2,2) = (-far+near)/(far-near);
  _projection(2,3) = (-2.f*far*near)/(far-near);
  _projection(3,2) = -1.f;
  _projection(3,3) = 0.f;
}
void Camera::ortho(float left,float right,float bottom,float top,float near,float far) {
  _projection = Matrix44f(1.f);
  _projection(0,0) = 2.f/(right-left);
  _projection(1,1) = 2.f/(top-bottom);
  _projection(2,2) = -2.f/(far-near);
  _projection(0,3) = -(right+left)/(right-left);
  _projection(1,3) = -(top+bottom)/(top-bottom);
  _projection(2,3) = -(far+near)/(far-near);
}
void Camera::pixels() {
  ortho(0.f,(float)Window::width(),(float)Window::height(),0.f);
}

bool Camera::worldToScreen(const Vector3f& p,Vector2f& wtr) const {
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
    Vector3f p(((c&0x1) ? i.min().x() : i.max().x()),
      ((c&0x2) ? i.min().y() : i.max().y()),
               ((c&0x4) ? i.min().z() : i.max().z()));
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
