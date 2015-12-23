#ifndef DEF_L_Camera
#define DEF_L_Camera

#include "Transform.h"

namespace L {
  class Camera : public Component {
    private:
      Transform* _transform;
    protected:
      Matrix44f _view, _projection, _viewProjection, _ray;
      Interval2f _viewport;
    public:
      Camera();
      void start();
      void prerender();

      inline void viewport(const Interval2f& i) {_viewport = i;}

      void perspective(float fovy, float aspect, float near, float far); // 3D perspective
      void ortho(float left, float right, float bottom, float top, float near = -1, float far = 1);
      void pixels(); // Maps to window's pixels (origins at top-left pixel)

      bool worldToScreen(const Vector3f&, Vector2f&) const; // Set the screen-space vector for that world space vector, returns false if behind camera
      Vector3f screenToRay(const Vector2f&) const; // Returns direction vector from normalized screen position
      bool sees(const Interval3f&) const; // Checks if an interval can currently be seen by camera

      inline const Matrix44f& view() const {return _view;}
      inline const Matrix44f& projection() const {return _projection;}
      inline const Matrix44f& viewProjection() const {return _viewProjection;}
  };
}


#endif

