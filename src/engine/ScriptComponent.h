#pragma once

#include "Component.h"
#include "../script/Context.h"

namespace L {
  class ScriptComponent : public Component {
    L_COMPONENT(ScriptComponent)
  protected:
    Script::Context _context;
  public:
    ScriptComponent();
    void start();
    void load(const char* filename);
    void update();
  };
}
