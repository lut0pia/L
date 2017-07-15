#pragma once

#include "Component.h"
#include "Transform.h"
#include "Resource.h"
#include "../gl/Mesh.h"

namespace L {
  class LightComponent : public Component {
    L_COMPONENT(LightComponent)
  protected:
    Transform* _transform;
    Vector3f _position, _color, _direction, _relative_dir;
    float _linear_attenuation, _quadratic_attenuation, _inner_angle, _outer_angle;
    int _type;
  public:
    inline LightComponent() { point(Color::white, .7f, 1.8f); }

    virtual void updateComponents() override { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    void lateUpdate();

    void directional(const Color& color, const Vector3f& direction);
    void point(const Color& color, float linear_attenuation, float quadratic_attenuation);
    void spot(const Color& color, const Vector3f& direction, float linear_attenuation, float quadratic_attenuation, float inner_angle, float outer_angle = 0.f);

    void render();
    static GL::Program& program();
  };
}
