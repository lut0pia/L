#pragma once

#include "Component.h"
#include "../input/InputContext.h"

namespace L {
  class InputComponent : public TComponent<InputComponent> {
  protected:
    InputContext _context;
  public:
    static void script_registration();
    inline Handle<InputContext> context() const { return _context.handle(); }
  };
}
