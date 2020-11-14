#pragma once

#include "Transform.h"
#include "../engine/Resource.h"
#include "../rendering/UniformBuffer.h"
#include "../rendering/Framebuffer.h"
#include "../rendering/Material.h"
#include "../math/Interval.h"
#include "../time/Time.h"

namespace L {
  class Camera : public TComponent<Camera, ComponentFlag::WindowEvent> {
  protected:
    Transform* _transform;
    Matrix44f _view, _projection, _view_projection, _prev_view_projection, _ray;
    Interval2f _viewport;
    Interval2i _viewport_pixel;
    enum {
      Perspective,
      Ortho
    } _projection_type;
    float _fovy, _near, _far, _left, _right, _bottom, _top;
    Material _present_material;
    RenderCommandBuffer* _cmd_buffer;
    Framebuffer _geometry_buffer, _light_buffer;
    Time _framebuffer_mtime;
    UniformBuffer _shared_uniform;
  public:
    Camera();
    inline Camera(const Camera&) : Camera() { error("Camera component should not be copied."); }
    inline Camera& operator=(const Camera&) { error("Camera component should not be copied."); return *this; }

    virtual void update_components() override;
    static void script_registration();

    void resize_buffers();
    void event(const Window::Event&);
    void prerender(RenderCommandBuffer*);
    void present();

    void viewport(const Interval2f& viewport);
    void perspective(float fovy, float near, float far); // 3D perspective
    void ortho(float left, float right, float bottom, float top, float near = -1, float far = 1);
    void pixels(); // Maps to window's pixels (origins at top-left pixel)
    void update_projection();
    void update_viewport();

    bool world_to_screen(const Vector3f&, Vector2f&) const; // Finds the normalized screen position for that world space vector, returns false if behind camera
    Vector3f screen_to_ray(const Vector2f&) const; // Returns direction vector from normalized screen position
    Vector2f screen_to_pixel(const Vector2f&) const; // Returns pixel position from screen
    Vector2f pixel_to_screen(const Vector2f&) const; // Returns screen position from pixel
    Interval2i viewport_pixel() const;
    bool sees(const Interval3f&) const; // Checks if an interval can currently be seen by camera
    void frustum_planes(Vector4f[6]) const; // In world-space

    inline const Matrix44f& view() const { return _view; }
    inline const Matrix44f& projection() const { return _projection; }
    inline const Matrix44f& view_projection() const { return _view_projection; }
    inline Material& present_material() { return _present_material; }
    inline Framebuffer& geometry_buffer() { return _geometry_buffer; }
    inline const Framebuffer& geometry_buffer() const { return _geometry_buffer; }
    inline Framebuffer& light_buffer() { return _light_buffer; }
    inline const Framebuffer& light_buffer() const { return _light_buffer; }
    inline const UniformBuffer& shared_uniform() const { return _shared_uniform; }
    inline RenderCommandBuffer* cmd_buffer() const { return _cmd_buffer; }
    inline Time framebuffer_mtime() const { return _framebuffer_mtime; }
  };
}
