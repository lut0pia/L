#ifndef DEF_L_GL_Camera
#define DEF_L_GL_Camera

#include "PRS.h"
#include "../containers/Ref.h"

namespace L {
  namespace GL {
    class Camera {
      protected:
        Point3f _position, _lookat, _up, _forward, _right;
        Matrix44f _view, _projection;
        float _fovy, _aspect, _near, _far;

      public:
        Camera(const Point3f& position);

        void move(const Point3f& delta);
        void position(const Point3f& position);
        void lookat(const Point3f&);

        void phi(float);
        void theta(float);

        void perspective(float fovy, float aspect, float near, float far);
        void ortho(float left, float right, float bottom, float top, float near = -1, float far = 1);
        void pixels();
        Point3f screenToRay(const Point2f&) const;

        void update();
        const Point3f& position() const{return _position;}
        const Point3f& forward() const{return _forward;}
        const Matrix44f& view() {return _view;}
        const Matrix44f& projection() {return _projection;}
    };
  }
}

#endif





