#pragma once

#include "Transform.h"
#include "../engine/Resource.h"
#include "../math/Interval.h"
#include "../gl/Texture.h"
#include "../gl/GL.h"

namespace L {
  class Sprite : public Component {
    L_COMPONENT(Sprite)
      L_COMPONENT_HAS_RENDER(Sprite)
  private:
    Transform* _transform;
    Resource<GL::Texture> _texture;
    Interval2f _vertex, _uv;
  public:
    inline Sprite() : _vertex(Vector2f(-1.f, -1.f), Vector2f(1.f, 1.f)), _uv(Vector2f(0.f, 0.f), Vector2f(1.f, 1.f)) {}
    inline Sprite(const Sprite&) : Sprite() { L_ERROR("Sprite component should not be copied."); }
    inline Sprite& operator=(const Sprite& other) { L_ERROR("Sprite component should not be copied."); }

    inline void update_components() override { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    void render(const Camera&);

    inline void texture(const char* filename) { _texture = Resource<GL::Texture>::get(filename); }
    inline void vertex(const Interval2f& v) { _vertex = v; }
    inline void uv(const Interval2f& u) { _uv = u; }
  };
}
