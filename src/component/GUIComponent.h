#pragma once

#include "Component.h"
#include "../rendering/Material.h"
#include "../engine/Resource.h"

namespace L {
  class GUIComponent : public Component {
    L_COMPONENT(GUIComponent)
      L_COMPONENT_HAS_GUI(GUIComponent)
  protected:
    Camera* _camera;
    Material _material;
    Vector2i _offset = 0;
    Vector2f _viewport_anchor = 0.f;
    Vector2f _anchor = 0.f;
    Vector2f _scale = 1.f;
  public:
    virtual void update_components() override;
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    void gui(const Camera&);

    inline Material* material() { return &_material; }
    inline void offset(const Vector2i& offset) { _offset = offset; }
    inline void viewport_anchor(const Vector2f& viewport_anchor) { _viewport_anchor = viewport_anchor; }
    inline void anchor(const Vector2f& anchor) { _anchor = anchor; }
    inline void scale(const Vector2f& scale) { _scale = scale; }
  };
}
