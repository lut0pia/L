#include "InputComponent.h"

using namespace L;

void InputComponent::script_registration() {
  InputContext::script_registration();

  L_COMPONENT_BIND(InputComponent, "input");
  L_SCRIPT_RETURN_METHOD(InputComponent, "context", 0, _context.handle());
}
