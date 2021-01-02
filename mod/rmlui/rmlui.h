#pragma once

#include <L/src/macros.h>

L_PUSH_NO_WARNINGS

#define RMLUI_STATIC_LIB
#define RMLUI_USE_CUSTOM_RTTI
#define RMLUI_NO_THIRDPARTY_CONTAINERS
#include <RmlUi/Core.h>
#include <RmlUi/Controls.h>

L_POP_NO_WARNINGS

typedef Rml::Core::ObserverPtr<Rml::Core::Element> RmlUiSafeElement;
