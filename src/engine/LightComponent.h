#pragma once

#include "Component.h"
#include "Transform.h"
#include "Resource.h"
#include "../gl/Mesh.h"
#include "../gl/Program.h"

namespace L {
  class LightComponent : public Component {
    L_COMPONENT(LightComponent)
  protected:
    Transform* _transform;
    Vector3f _position, _color, _direction, _relative_dir;
    float _intensity, _radius, _inner_angle, _outer_angle;
    int _type;
  public:
    inline LightComponent() { point(Color::white); }

    virtual void updateComponents() override { _transform = entity()->requireComponent<Transform>(); }
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    void lateUpdate();

    void directional(const Color& color, const Vector3f& direction, float intensity = 1.f);
    void point(const Color& color, float intensity = 1.f, float radius = 1.f);
    void spot(const Color& color, const Vector3f& direction, float intensity = 1.f, float radius = 1.f, float inner_angle = .5f, float outer_angle = 0.f);

    void render();
    static GL::Program& program();
  };
}
