#ifndef DEF_L_GL_Camera
#define DEF_L_GL_Camera

#include "PRS.h"
#include "../containers/Ref.h"

namespace L {
  namespace GL {
    class Camera {
      protected:
        Point3f _position, _lookat, _up, _forward, _right;
        Matrix44f _view;
        float _fovy, _aspect, _nearLimit, _farLimit;

      public:
        Camera(const Point3f& position, float fovy = 70, float aspect = 16.0/9.0, float nearLimit = .01, float farLimit = 512);

        void move(const Point3f& delta);
        void position(const Point3f& position);
        const Point3f& position() const{return _position;}
        const Point3f& forward() const{return _forward;}
        void lookat(const Point3f&);

        void phi(float);
        void theta(float);

        void fovy(float fovy);
        void aspect(float aspect);
        void clipping(float near, float far);

        void update();
        void place();
        const Matrix44f& view() {return _view;}
    };
  }
}

#endif





