#pragma once

#include "Transform.h"
#include "../font/Font.h"
#include "../rendering/Framebuffer.h"
#include "../rendering/Texture.h"
#include "../math/Interval.h"

namespace L {
  class Camera : public Component {
    L_COMPONENT(Camera)
      L_COMPONENT_HAS_WIN_EVENT(Camera)
  private:
    Transform* _transform;
  protected:
    Matrix44f _view, _projection, _viewProjection, _prevViewProjection, _ray;
    Interval2f _viewport;
    enum {
      Perspective,
      Ortho
    } _projectionType;
    float _fovy, _near, _far, _left, _right, _bottom, _top;
    VkCommandBuffer _cmd_buffer;
    VkViewport _vk_viewport;
    Framebuffer _geometry_buffer, _light_buffer;
  public:
    Camera();
    inline Camera(const Camera&) : Camera() { error("Camera component should not be copied."); }
    inline Camera& operator=(const Camera& other) { error("Camera component should not be copied."); return *this; }

    virtual void update_components() override;
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    void resize_buffers();
    void event(const Window::Event&);
    void prerender(VkCommandBuffer);
    void postrender();
    void present();

    void viewport(const Interval2f& viewport);
    void perspective(float fovy, float near, float far); // 3D perspective
    void ortho(float left, float right, float bottom, float top, float near = -1, float far = 1);
    void pixels(); // Maps to window's pixels (origins at top-left pixel)
    void update_projection();
    void update_viewport();

    void draw_text(int x, int y, const char* text, Resource<Font> font);
    void draw_image(int x, int y, Resource<Texture> texture);

    bool worldToScreen(const Vector3f&, Vector2f&) const; // Finds the normalized screen position for that world space vector, returns false if behind camera
    Vector3f screenToRay(const Vector2f&) const; // Returns direction vector from normalized screen position
    Vector2f screenToPixel(const Vector2f&) const; // Returns pixel position from NDC
    Interval2i viewportPixel() const;
    bool sees(const Interval3f&) const; // Checks if an interval can currently be seen by camera
    void frustum_planes(Vector4f[6]) const; // In world-space

    inline const Matrix44f& view() const { return _view; }
    inline const Matrix44f& projection() const { return _projection; }
    inline const Matrix44f& viewProjection() const { return _viewProjection; }
    inline VkCommandBuffer cmd_buffer() const { return _cmd_buffer; }
  };
}
