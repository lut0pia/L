#pragma once

#include "Component.h"
#include "../math/Matrix.h"
#include "../math/Quaternion.h"
#include "../math/Vector.h"

namespace L {
  class Transform : public Component {
      L_COMPONENT(Transform)
    private:
      Transform* _parent;
      Vector3f _translation;
      Quatf _rotation;
    public:
      inline Transform() : _parent(nullptr), _translation(0,0,0) {}
      inline Vector3f absolutePosition() const {return (_parent)?(_parent->absolutePosition()+parentRotation().rotate(_translation)):_translation;}
      inline Vector3f toAbsolute(const Vector3f& v) const {return absolutePosition()+_rotation.rotate(v);}
      inline Quatf absoluteRotation() const {return (_parent)?_parent->absoluteRotation():_rotation;}
      inline Quatf parentRotation() const {return (_parent)?_parent->absoluteRotation():Quatf();}
      inline void rotate(const Quatf& q) {_rotation = _rotation * q;}
      inline void rotate(const Vector3f& v, float d) {rotate(Quatf(v,d));}
      inline void rotateAbsolute(const Quatf& q){ _rotation = q * _rotation; } // Wrong
      inline void rotateAbsolute(const Vector3f& v,float d) { rotateAbsolute(Quatf(v,d)); }
      inline void phiLook(float d) {rotate(Vector3f(1,0,0),d);}
      inline void thetaLook(float d) {rotate(Vector3f(0,0,1),d);}
      inline void move(const Vector3f& d) {_translation += _rotation.rotate(d);}
      inline void moveAbsolute(const Vector3f& v) {_translation += parentRotation().inverse().rotate(v);}

      inline void parent(Transform* p) {_parent = p;}
      inline Transform* parent() const {return _parent;}
      inline Vector3f right() const {return absoluteRotation().rotate(Vector3f(1,0,0));}
      inline Vector3f forward() const {return absoluteRotation().rotate(Vector3f(0,1,0));}
      inline Vector3f up() const {return absoluteRotation().rotate(Vector3f(0,0,1));}
      inline Matrix44f matrix() const {return Matrix44f::translation(absolutePosition())*Matrix44f::orientation(right(),forward(),up());}
  };
}
