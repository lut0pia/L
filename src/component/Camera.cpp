#include "Camera.h"

#ifdef near
#undef near
#undef far
#endif

#include "../constants.h"
#include "../gl/Buffer.h"
#include "../gl/GL.h"
#include "../gl/Program.h"
#include "LightComponent.h"
#include "../system/Window.h"
#include "../engine/Engine.h"
#include "../engine/SharedUniform.h"

using namespace L;

Camera::Camera() :
  _viewport(Vector2f(0,0),Vector2f(1,1)),
  _gbuffer({&_gcolor,&_gnormal}, &_gdepth),
  _lbuffer({&_lcolor}) {
  resize_buffers();
  _gcolor.parameter(GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  _gcolor.parameter(GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  _gnormal.parameter(GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  _gnormal.parameter(GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  _gdepth.parameter(GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  _gdepth.parameter(GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  _lcolor.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  _lcolor.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Check framebuffer states
  _gbuffer.check();
  _lbuffer.check();
}

void Camera::update_components() {
  _transform = entity()->requireComponent<Transform>();
}
static const Symbol perspective_symbol("perspective"), ortho_symbol("ortho");
Map<Symbol, Var> Camera::pack() const {
  Map<Symbol, Var> data;
  data["projection"] = (_projectionType==Perspective) ? perspective_symbol : ortho_symbol;
  data["fovy"] = _fovy;
  data["near"] = _near;
  data["far"] = _far;
  data["left"] = _left;
  data["right"] = _right;
  data["bottom"] = _bottom;
  data["top"] = _top;
  return data;
}
void Camera::unpack(const Map<Symbol, Var>& data) {
  Symbol projection;
  unpack_item(data, "projection", projection);
  _projectionType = (projection==perspective_symbol) ? Perspective : Ortho;
  unpack_item(data, "fovy", _fovy);
  unpack_item(data, "near", _near);
  unpack_item(data, "far", _far);
  unpack_item(data, "left", _left);
  unpack_item(data, "right", _right);
  unpack_item(data, "bottom", _bottom);
  unpack_item(data, "top", _top);
  resize_buffers();
  updateProjection();
}
void Camera::script_registration() {
  L_COMPONENT_BIND(Camera, "camera");
  L_COMPONENT_METHOD(Camera, "perspective", 3, perspective(c.local(0).get<float>(), c.local(1).get<float>(), c.local(2).get<float>()));
  L_COMPONENT_METHOD(Camera, "ortho", 4, ortho(c.local(0).get<float>(), c.local(1).get<float>(), c.local(2).get<float>(), c.local(3).get<float>()));
  L_COMPONENT_METHOD(Camera, "viewport", 4, viewport(Interval2f(Vector2f(c.local(0).get<float>(), c.local(1).get<float>()), Vector2f(c.local(2).get<float>(), c.local(3).get<float>()))));
}

void Camera::resize_buffers() {
  const Vector2f viewport_size(_viewport.size());
  const GLsizei viewport_width(Window::width()*viewport_size.x()), viewport_height(Window::height()*viewport_size.y());
  _gcolor.image2D(0, GL_RGBA16F, viewport_width, viewport_height, 0, GL_RGBA, GL_FLOAT);
  _gnormal.image2D(0, GL_RGBA16F, viewport_width, viewport_height, 0, GL_RGBA, GL_FLOAT);
  _gdepth.image2D(0, GL_DEPTH_COMPONENT24, viewport_width, viewport_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT);
  _lcolor.image2D(0, GL_RGB16F, viewport_width, viewport_height, 0, GL_RGB, GL_FLOAT);
}
void Camera::event(const Window::Event& e) {
  if(e.type == Window::Event::Resize) {
    resize_buffers();
    updateProjection();
  }
}
void Camera::prerender() {
  static Matrix44f camOrient(orientation_matrix(Vector3f(1,0,0),Vector3f(0,0,1),Vector3f(0,-1,0)).inverse());
  Matrix44f orientation(orientation_matrix(_transform->right(),_transform->forward(),_transform->up()));
  _view = camOrient * _transform->matrix().inverse();
  _prevViewProjection = _viewProjection;
  _viewProjection = _projection*_view;
  _ray = orientation*_projection.inverse();
  glViewport(0,0,_gcolor.width(),_gcolor.height());
  Engine::shared_uniform().subData(L_SHAREDUNIFORM_VIEW,_view);
  Engine::shared_uniform().subData(L_SHAREDUNIFORM_INVVIEW,_view.inverse());
  Engine::shared_uniform().subData(L_SHAREDUNIFORM_VIEWPROJ,_viewProjection);
  Engine::shared_uniform().subData(L_SHAREDUNIFORM_INVVIEWPROJ,_viewProjection.inverse());
  Engine::shared_uniform().subData(L_SHAREDUNIFORM_PREVVIEWPROJ,_prevViewProjection);
  Engine::shared_uniform().subData(L_SHAREDUNIFORM_EYE,_transform->position());
  Engine::shared_uniform().subData(L_SHAREDUNIFORM_VIEWPORT, Vector4f(_viewport.min().x(),_viewport.min().y(),_viewport.max().x(),_viewport.max().y()));
  _gbuffer.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}
void Camera::postrender(){
  _gbuffer.unbind();
  _lbuffer.bind();
  const Interval2i vp(viewportPixel());
  const Vector2i vpSize(vp.size());
  glViewport(vp.min().x(), vp.min().y(), vpSize.x(), vpSize.y());
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE); // Additive blending

  Resource<GL::Program>& light_program(LightComponent::program());
  if(light_program) {
    light_program->use();
    light_program->uniform("color_buffer", _gcolor, GL_TEXTURE0);
    light_program->uniform("normal_buffer", _gnormal, GL_TEXTURE1);
    light_program->uniform("depth_buffer", _gdepth, GL_TEXTURE2);

    ComponentPool<LightComponent>::iterate([](LightComponent& light) {
      light.render();
    });
  }

  _lbuffer.unbind();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // For GUI alpha
  static GL::Program final_shader(GL::Shader(
    "#version 330 core\n"
    "const vec2 vertices[] = vec2[]("
    "vec2(-1.f,-1.f),vec2(3.f,-1.f),vec2(-1.f,3.f));"
    "out vec2 ftexcoords;"
    "void main(){"
    "ftexcoords = vertices[gl_VertexID]*.5f+.5f;"
    "gl_Position = vec4(vertices[gl_VertexID],0.f,1.f);"
    "}", GL_VERTEX_SHADER),
    GL::Shader(
      "#version 330 core\n"
      L_SHAREDUNIFORM
      L_SHADER_LIB
      "in vec2 ftexcoords;"
      "out vec4 fragcolor;"
      "uniform sampler2D color_buffer;"
      "void main(){"
      "vec3 color = texture(color_buffer,ftexcoords).rgb;"
      "color = pow(color, vec3(1.f/2.2f));" // Gamma correction
      "fragcolor = vec4(deband(color),1.f);"
      "}", GL_FRAGMENT_SHADER));

  final_shader.use();
  final_shader.uniform("color_buffer", _lcolor);
  GL::draw(GL_TRIANGLES, 3);
}

void Camera::viewport(const Interval2f& i) {
  _viewport = i;
  resize_buffers();
  updateProjection();
}
void Camera::perspective(float fovy,float near,float far) {
  _projectionType = Perspective;
  _fovy = fovy; _near = near; _far = far;
  updateProjection();
}
void Camera::ortho(float left,float right,float bottom,float top,float near,float far) {
  _projectionType = Ortho;
  _left = left; _right = right; _bottom = bottom; _top = top; _near = near; _far = far;
  updateProjection();
}
void Camera::pixels() {
  ortho(0.f,(float)Window::width(),(float)Window::height(),0.f);
}
void Camera::updateProjection(){
  switch(_projectionType){
    case L::Camera::Perspective:
    {
      _projection = Matrix44f(1.f);
      const Vector2f viewportSize(_viewport.size());
      const float aspect((viewportSize.x()*Window::width())/(viewportSize.y()*Window::height())),
        top(_near*tan(_fovy*(PI<float>()/360.f))),
        right(top*aspect);
      _projection(0,0) = _near/right;
      _projection(1,1) = _near/top;
      _projection(2,2) = (-_far+_near)/(_far-_near);
      _projection(2,3) = (-2.f*_far*_near)/(_far-_near);
      _projection(3,2) = -1.f;
      _projection(3,3) = 0.f;
    }
    break;
    case L::Camera::Ortho:
    {
      _projection = Matrix44f(1.f);
      _projection(0,0) = 2.f/(_right-_left);
      _projection(1,1) = 2.f/(_top-_bottom);
      _projection(2,2) = 2.f/(_far-_near);
      _projection(0,3) = -(_right+_left)/(_right-_left);
      _projection(1,3) = -(_top+_bottom)/(_top-_bottom);
      _projection(2,3) = -(_far+_near)/(_far-_near);
    }
    break;
  }
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
Vector2f Camera::screenToPixel(const Vector2f& v) const {
  const Vector2f viewportSize(_viewport.size());
  return (v*Vector2f(.5f, -.5f)+.5f)*Vector2f(viewportSize.x()*Window::width(), viewportSize.y()*Window::height());
}
Interval2i Camera::viewportPixel() const {
  const Vector2i windowSize(Window::width(), Window::height());
  return Interval2i(_viewport.min()*windowSize, _viewport.max()*windowSize);
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
