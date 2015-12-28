#ifndef DEF_L_Transform
#define DEF_L_Transform

#include "Component.h"
#include "../math/Matrix.h"

namespace L {
  class Transform : public Component {
      L_COMPONENT(Transform)
    private:
      Transform* _parent;
      Matrix44f _relative, _absolute;
      uint _absoluteFrame;
    public:
      inline Transform() : _parent(NULL), _relative(Matrix44f::identity()), _absoluteFrame(-1) {}
      inline void parent(Transform* p) {_parent = p;}
      inline Transform* parent()const {return _parent;}
      inline void relative(const Matrix44f& m) {_relative = m; _absoluteFrame = -1;}
      inline void mult(const Matrix44f& m) { _relative = _relative * m; _absoluteFrame = -1;}
      inline void rotate(const Vector3f& v, float d) {mult(Matrix44f::rotation(v,d));}
      inline void phiLook(float d) {rotate(Vector3f(1,0,0),d);}
      inline void thetaLook(float d) {rotate(Vector3f(0,0,1),d);}
      inline void move(const Vector3f& d) {mult(Matrix44f::translation(d));}

      inline const Matrix44f& relative() {return _relative;}
      Vector3f right();
      Vector3f forward();
      Vector3f up();
      Vector3f position();
      const Matrix44f& absolute();
  };
}


#endif

