#pragma once

#include "Transform.h"
#include "../math/Interval.h"
#include "../gl/Texture.h"
#include "../gl/GL.h"

namespace L {
  class Sprite : public Component {
    L_COMPONENT(Sprite)
  private:
    Transform* _transform;
    Ref<GL::Texture> _texture;
    Interval2f _vertex,_uv;
  public:
    inline Sprite() : _vertex(Vector2f(-.5f,-.5f),Vector2f(.5f,.5f)),_uv(Vector2f(0.f,0.f),Vector2f(1.f,1.f)){}
    inline void updateComponents() { _transform = entity()->requireComponent<Transform>(); }
    void render(const Camera&);
    inline void texture(const char* filename) { _texture = Engine::texture(filename); }
    inline void texture(const Ref<GL::Texture>& tex) { _texture = tex; }
    inline const Ref<GL::Texture>& texture() { return _texture; }
    inline void vertex(const Interval2f& v) { _vertex = v; }
    inline void uv(const Interval2f& u) { _uv = u; }
  };
}
