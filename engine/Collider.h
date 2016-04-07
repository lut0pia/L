#ifndef DEF_L_Collider
#define DEF_L_Collider

#include "Transform.h"
#include "RigidBody.h"
#include "../math/Interval.h"

namespace L {
  class Collider : public Component {
      L_COMPONENT(Collider)
    protected:
      Transform* _transform;
      RigidBody* _rigidbody;
      union {
        Interval3f _box;
        struct {
          Vector3f _center;
          float _radius;
        };
      };
      enum {
        Box,Sphere
      } _type;
    public:
      void start();
      void update() {
        // TODO: replace with broadphase
        Pool<Collider>::global.foreach([this](Collider& other) {
          if(&other!=this && other.entity()!=entity() && (_rigidbody || other._rigidbody) && &other<this)
            checkCollision(*this,other);
        });
      }
      inline void box(const Interval3f& b) {_type = Box; _box = b;}
      inline void sphere(const Vector3f& center, float radius) {_type = Sphere; _center = center; _radius = radius;}
      Interval1f project(const Vector3f& axis) const;
      Interval3f boundingBox() const;
      Vector3f leastToAxis(const Vector3f& axis) const;
      void render(const Camera& camera);
      void checkCollision(const Collider& a, const Collider& b);
  };
}


#endif

