#pragma once

#include "Transform.h"
#include "../gl/FrameBuffer.h"
#include "../math/Interval.h"

namespace L {
  class Camera : public Component {
    L_COMPONENT(Camera)
  private:
    Transform* _transform;
  protected:
    Matrix44f _view,_projection,_viewProjection,_prevViewProjection,_ray;
    Interval2f _viewport;
    enum{
      Perspective,
      Ortho
    } _projectionType;
    float _fovy,_near,_far,_left,_right,_bottom,_top;
    GL::Texture _gcolor, _gnormal, _gdepth, _pp_color[2];
    GL::FrameBuffer _gbuffer, _pp_buffer[2];
    uint32_t _pp_index : 1;
  public:
    Camera();
    inline Camera(const Camera&) : Camera(){ L_ERROR("Camera component should not be copied."); }
    inline Camera& operator=(const Camera& other){ L_ERROR("Camera component should not be copied."); }

    virtual void updateComponents() override;
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    void resize_buffers();
    void event(const Window::Event&);
    void prerender();
    void postrender();

    void viewport(const Interval2f& viewport);
    void perspective(float fovy,float near,float far); // 3D perspective
    void ortho(float left,float right,float bottom,float top,float near = -1,float far = 1);
    void pixels(); // Maps to window's pixels (origins at top-left pixel)
    void updateProjection();

    bool worldToScreen(const Vector3f&,Vector2f&) const; // Finds the normalized screen position for that world space vector, returns false if behind camera
    Vector3f screenToRay(const Vector2f&) const; // Returns direction vector from normalized screen position
    Vector2f screenToPixel(const Vector2f&) const; // Returns pixel position from NDC
    Interval2i viewportPixel() const;
    bool sees(const Interval3f&) const; // Checks if an interval can currently be seen by camera

    inline const Matrix44f& view() const { return _view; }
    inline const Matrix44f& projection() const { return _projection; }
    inline const Matrix44f& viewProjection() const { return _viewProjection; }
  };
}
