#pragma once

#include "Component.h"
#include "../rendering/Material.h"
#include "../engine/Resource.h"

namespace L {
  class PostProcessComponent : public TComponent<PostProcessComponent> {
  protected:
    Material _material;
  public:
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;
    static void script_registration();

    void render(const Camera& camera, const RenderPass& render_pass);

    inline Material* material() { return &_material; }
  };
}
