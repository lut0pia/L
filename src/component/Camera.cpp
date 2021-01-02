#include "Camera.h"

#include "../engine/Resource.inl"
#include "../rendering/Material.h"
#include "../system/Window.h"
#include "../engine/Engine.h"
#include "../engine/Settings.h"
#include "../rendering/shader_lib.h"

using namespace L;

static const float& screen_percentage(Settings::get_float("screen-percentage", 1.0f));

Camera::Camera() :
  _viewport(Vector2f(0.f, 0.f), Vector2f(1.f, 1.f)),
  _framebuffer_mtime(Time::now()),
  _shared_uniform(L_SHAREDUNIFORM_SIZE) {
  update_viewport();
  resize_buffers();
}
Camera::~Camera() {
  destroy_buffers();
}

void Camera::update_components() {
  _transform = entity()->require_component<Transform>();
}
void Camera::script_registration() {
  L_COMPONENT_BIND(Camera, "camera");
  L_SCRIPT_RETURN_METHOD(Camera, "present_material", 0, present_material().handle());
  L_SCRIPT_METHOD(Camera, "perspective", 3, perspective(c.param(0), c.param(1), c.param(2)));
  L_SCRIPT_METHOD(Camera, "ortho", 4, ortho(c.param(0), c.param(1), c.param(2), c.param(3)));
  L_SCRIPT_METHOD(Camera, "viewport", 4, viewport(Interval2f(Vector2f(c.param(0).get<float>(), c.param(1).get<float>()), Vector2f(c.param(2).get<float>(), c.param(3).get<float>()))));

  L_SCRIPT_RETURN_METHOD(Camera, "screen_to_ray", 1, screen_to_ray(c.param(0)));
  L_SCRIPT_RETURN_METHOD(Camera, "screen_to_pixel", 1, screen_to_pixel(c.param(0)));
  L_SCRIPT_RETURN_METHOD(Camera, "pixel_to_screen", 1, pixel_to_screen(c.param(0)));
}

void Camera::destroy_buffers() {

  if(_geometry_buffer) {
    Renderer::get()->destroy_framebuffer(_geometry_buffer);
    _geometry_buffer = nullptr;
  }

  if(_light_buffer) {
    Renderer::get()->destroy_framebuffer(_light_buffer);
    _light_buffer = nullptr;
  }

  if(_color_texture) {
    Renderer::get()->destroy_texture(_color_texture);
    _color_texture = nullptr;
  }

  if(_normal_texture) {
    Renderer::get()->destroy_texture(_normal_texture);
    _normal_texture = nullptr;
  }

  if(_light_texture) {
    Renderer::get()->destroy_texture(_light_texture);
    _light_texture = nullptr;
  }

  if(_depth_texture) {
    Renderer::get()->destroy_texture(_depth_texture);
    _depth_texture = nullptr;
  }
}
void Camera::resize_buffers() {
  destroy_buffers();

  const Vector2f viewport_size = _viewport.size();
  const uint32_t viewport_width = uint32_t(Window::width() * viewport_size.x() * screen_percentage);
  const uint32_t viewport_height = uint32_t(Window::height() * viewport_size.y() * screen_percentage);
  _color_texture = Renderer::get()->create_texture(viewport_width, viewport_height, RenderFormat::R8G8B8A8_UNorm);
  _normal_texture = Renderer::get()->create_texture(viewport_width, viewport_height, RenderFormat::R16G16B16A16_UNorm);
  _light_texture = Renderer::get()->create_texture(viewport_width, viewport_height, RenderFormat::R16G16B16A16_SFloat);
  _depth_texture = Renderer::get()->create_texture(viewport_width, viewport_height, RenderFormat::D24_UNorm_S8_UInt);

  const TextureImpl* geometry_textures[] = {_color_texture, _normal_texture, _depth_texture};
  const TextureImpl* light_textures[] = {_light_texture, _depth_texture};
  _geometry_buffer = Renderer::get()->create_framebuffer(Renderer::get()->get_geometry_pass(), geometry_textures, 3);
  _light_buffer = Renderer::get()->create_framebuffer(Renderer::get()->get_light_pass(), light_textures, 2);
  _framebuffer_mtime = Time::now();
}
void Camera::event(const Window::Event& e) {
  if(e.type == Window::Event::Type::Resize) {
    resize_buffers();
    update_projection();
    update_viewport();
  }
}
void Camera::prerender(RenderCommandBuffer* cmd_buffer) {
  L_SCOPE_MARKER("Camera::prerender");
  static const Matrix44f cam_nz2y = orientation_matrix(Vector3f(1.f, 0.f, 0.f), Vector3f(0.f, 0.f, 1.f), Vector3f(0.f, -1.f, 0.f)).transpose();
  const Matrix44f orientation = orientation_matrix(_transform->right(), _transform->forward(), _transform->up());
  const Vector2f viewport_size = _viewport.size();
  const uint32_t viewport_width = uint32_t(Window::width() * viewport_size.x() * screen_percentage);
  const uint32_t viewport_height = uint32_t(Window::height() * viewport_size.y() * screen_percentage);

  _view = cam_nz2y * _transform->matrix().inverse();
  _prev_view_projection = _view_projection;
  _view_projection = _projection * _view;
  _ray = orientation * cam_nz2y.transpose() * _projection.inverse();
  _shared_uniform.load_item(Engine::frame(), L_SHAREDUNIFORM_FRAME);
  _shared_uniform.load_item(_view, L_SHAREDUNIFORM_VIEW);
  _shared_uniform.load_item(_view.inverse(), L_SHAREDUNIFORM_INVVIEW);
  _shared_uniform.load_item(_projection, L_SHAREDUNIFORM_PROJECTION);
  _shared_uniform.load_item(_view_projection, L_SHAREDUNIFORM_VIEWPROJ);
  _shared_uniform.load_item(_view_projection.inverse(), L_SHAREDUNIFORM_INVVIEWPROJ);
  _shared_uniform.load_item(_prev_view_projection, L_SHAREDUNIFORM_PREVVIEWPROJ);
  _shared_uniform.load_item(_transform->position(), L_SHAREDUNIFORM_EYE);
  _shared_uniform.load_item(Vector4f(float(Window::width()), float(Window::height()), float(viewport_width), float(viewport_height)), L_SHAREDUNIFORM_SCREEN);
  _shared_uniform.load_item(Vector4f(_viewport.min().x(), _viewport.min().y(), _viewport.max().x(), _viewport.max().y()), L_SHAREDUNIFORM_VIEWPORT);
  _shared_uniform.load_item(Vector4f(float(viewport_width), float(viewport_height), 1.f / viewport_width, 1.f / viewport_height), L_SHAREDUNIFORM_VIEWPORT_PIXEL_SIZE);

  _cmd_buffer = cmd_buffer;

  Renderer::get()->set_viewport(cmd_buffer, Interval2i(0, Vector2i(viewport_width, viewport_height)));

  Renderer::get()->reset_scissor(cmd_buffer);

  _present_material.update();
}
void Camera::present() {
  Renderer::get()->set_viewport(_cmd_buffer, _viewport_pixel);
  _present_material.draw(*this, Renderer::get()->get_present_pass());
}

void Camera::viewport(const Interval2f& i) {
  _viewport = i;
  resize_buffers();
  update_projection();
  update_viewport();
}
void Camera::perspective(float fovy, float near, float far) {
  _projection_type = Perspective;
  _fovy = fovy; _near = near; _far = far;
  update_projection();
}
void Camera::ortho(float left, float right, float bottom, float top, float near, float far) {
  _projection_type = Ortho;
  _left = left; _right = right; _bottom = bottom; _top = top; _near = near; _far = far;
  update_projection();
}
void Camera::pixels() {
  ortho(0.f, (float)Window::width(), (float)Window::height(), 0.f);
}
void Camera::update_projection() {
  static const float pi = 3.14159265358979323846f;
  switch(_projection_type) {
    case L::Camera::Perspective:
    {
      _projection = Matrix44f(1.f);
      const Vector2f viewportSize(_viewport.size());
      const float aspect((viewportSize.x() * Window::width()) / (viewportSize.y() * Window::height())),
        top(_near * tan(_fovy * (pi / 360.f))),
        right(top * aspect);
      _projection(0, 0) = _near / right;
      _projection(1, 1) = -_near / top;
      _projection(2, 2) = -_far / (_far - _near);
      _projection(2, 3) = -_far * _near / (_far - _near);
      _projection(3, 2) = -1.f;
      _projection(3, 3) = 0.f;
    }
    break;
    case L::Camera::Ortho:
    {
      _projection = Matrix44f(1.f);
      _projection(0, 0) = 2.f / (_right - _left);
      _projection(1, 1) = 2.f / (_top - _bottom);
      _projection(2, 2) = 2.f / (_far - _near);
      _projection(0, 3) = -(_right + _left) / (_right - _left);
      _projection(1, 3) = -(_top + _bottom) / (_top - _bottom);
      _projection(2, 3) = -(_far + _near) / (_far - _near);
    }
    break;
  }
}
void Camera::update_viewport() {
  _viewport_pixel =
    Interval2i(
      Vector2i(int32_t(Window::width() * _viewport.min().x()), int32_t(Window::height() * _viewport.min().y())),
      Vector2i(int32_t(Window::width() * _viewport.max().x()), int32_t(Window::height() * _viewport.max().y())));
}

bool Camera::world_to_screen(const Vector3f& p, Vector2f& wtr) const {
  const Vector4f q = _view_projection * p;
  if(q.w() > 0) { // We do not want values behind the camera
    wtr = Vector3f(q) / q.w();
  } else {
    return false;
  }
  return true;
}
Vector3f Camera::screen_to_ray(const Vector2f& p) const {
  return Vector3f(_ray * Vector4f(p.x(), p.y(), -1.f, 1.f)).normalize();
}
Vector2f Camera::screen_to_pixel(const Vector2f& v) const {
  const Vector2f viewport_size = _viewport.size() * Vector2f(float(Window::width()), float(Window::height()));
  return (v * Vector2f(.5f, -.5f) + .5f) * viewport_size;
}
Vector2f Camera::pixel_to_screen(const Vector2f& v) const {
  const Vector2f viewport_size = _viewport.size() * Vector2f(float(Window::width()), float(Window::height()));
  return (v / viewport_size) * 2.f - 1.f;
}
Interval2i Camera::viewport_pixel() const {
  const Vector2i window_size(Window::width(), Window::height());
  return Interval2i(_viewport.min() * window_size, _viewport.max() * window_size);
}
bool Camera::sees(const Interval3f& i) const {
  static const Interval3f ndc(Vector3f(-1.f, -1.f, -1.f), Vector3f(1.f, 1.f, 1.f));
  Interval3f projected;
  for(int c(0); c < 8; c++) { // Cycle through corners of the interval
    Vector3f p(((c & 0x1) ? i.min().x() : i.max().x()),
      ((c & 0x2) ? i.min().y() : i.max().y()),
      ((c & 0x4) ? i.min().z() : i.max().z()));
    Vector4f q(_view_projection * p);
    if(q.w() > 0) { // We do not want values behind the camera
      Vector3f r(Vector3f(q) / q.w()); // Compute NDC coordinates
      if(abs(r.x()) < 1.f && abs(r.y()) < 1.f && abs(r.z()) < 1.f) { // The point is clearly in view
        return true;
      } else if(c == 0) {
        projected = r;
      } else { // The shape could go through the view
        projected.add(r);
        if(ndc.overlaps(projected))
          return true;
      }
    }
  }
  return false;
}
void Camera::frustum_planes(Vector4f planes[6]) const {
  const Matrix44f& m(_view_projection);
  planes[0] = m.row(3) + m.row(0);
  planes[1] = m.row(3) - m.row(0);
  planes[2] = m.row(3) + m.row(1);
  planes[3] = m.row(3) - m.row(1);
  planes[4] = m.row(3) + m.row(2);
  planes[5] = m.row(3) - m.row(2);
}
