#pragma once

#include "Component.h"
#include "../rendering/Material.h"
#include "../engine/Resource.h"

namespace L {
  class PostProcessComponent : public TComponent<PostProcessComponent> {
  protected:
    Material _material;
  public:
    static void script_registration();

    void render(const Camera& camera, const RenderPass& render_pass);

    inline Material& material() { return _material; }
  };
}
