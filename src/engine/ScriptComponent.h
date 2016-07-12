#pragma once

#include "Component.h"
#include "../script/Context.h"

namespace L {
  class ScriptComponent : public Component {
    L_COMPONENT(ScriptComponent)
  protected:
    Script::Context _context;
  public:
    void start();
    void load(const char* filename);
    void update();
    void event(const Window::Event&);
    void event(const Ref<Map<Var,Var>>&);
    static void init();
  };
}
