#pragma once

#include "Transform.h"
#include "Resource.h"
#include "../math/Interval.h"
#include "../gl/Texture.h"
#include "../gl/GL.h"

namespace L {
  class Sprite : public Component {
    L_COMPONENT(Sprite)
  private:
    Transform* _transform;
    Resource<GL::Texture> _texture;
    GL::Mesh _mesh;
    Interval2f _vertex, _uv;
  public:
    inline Sprite() : _vertex(Vector2f(-1.f, -1.f), Vector2f(1.f, 1.f)), _uv(Vector2f(0.f, 0.f), Vector2f(1.f, 1.f)) {
      update_mesh();
    }
    inline Sprite(const Sprite&) : Sprite() { L_ERROR("Sprite component should not be copied."); }
    inline Sprite& operator=(const Sprite& other) { L_ERROR("Sprite component should not be copied."); }

    inline void updateComponents() override { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    inline void texture(const char* filename) { _texture = Resource<GL::Texture>::get(filename); }
    void vertex(const Interval2f& v);
    void uv(const Interval2f& u);
    void update_mesh();
    void render(const Camera&);
  };
}
