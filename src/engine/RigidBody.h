#pragma once

#include "Transform.h"

namespace L {
  class RigidBody : public Component {
    L_COMPONENT(RigidBody)
  protected:
    Transform* _transform;
    Vector3f _speed,_rotSpeed;
    float _mass;
  public:
    void start() {
      _transform = entity()->requireComponent<Transform>();
      _mass = 1.f;
      _speed = Vector3f(0,0,0);
    }
    void update() {
      _transform->moveAbsolute(_speed*Engine::deltaSeconds());
      _speed += Engine::deltaSeconds()*Vector3f(0,0,-9.8f);
    }
    inline void addSpeed(const Vector3f& v){ _speed += v; }
    void collide(const Vector3f& impact,const Vector3f& normal) {
      if(_speed.dot(normal)<0.f) // Only
        _speed = normal.reflect(_speed)*-.5f;
    }
    static void collision(RigidBody* a,RigidBody* b,const Vector3f& impact,const Vector3f& normal) {
      if(a)a->collide(impact,normal);
      if(b)b->collide(impact,-normal);
    }
  };
}
