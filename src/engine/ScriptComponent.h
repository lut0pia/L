#ifndef DEF_L_ScriptComponent
#define DEF_L_ScriptComponent

#include "Transform.h"
#include "../script/Context.h"

namespace L {
  class ScriptComponent : public Component {
    L_COMPONENT(ScriptComponent)
  protected:
    Script::Context _context;
  public:
    ScriptComponent();
    void load(const char* filename);
    void update();
  };
}


#endif

