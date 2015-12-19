#ifndef DEF_L_Transform
#define DEF_L_Transform

#include "Component.h"
#include "../math/Matrix.h"

namespace L {
  class Transform : public Component {
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

      inline const Matrix44f& relative() {return _relative;}
      Vector3f right();
      Vector3f forward();
      Vector3f up();
      Vector3f position();
      const Matrix44f& absolute();
  };
}


#endif

