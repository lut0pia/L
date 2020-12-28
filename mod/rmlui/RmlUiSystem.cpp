#include "RmlUiSystem.h"

#include <L/src/dev/debug.h>
#include <L/src/time/Time.h>

using namespace L;

double RmlUiSystem::GetElapsedTime() {
  return Time::now().seconds_float();
}
bool RmlUiSystem::LogMessage(Rml::Core::Log::Type type, const Rml::Core::String& message) {
  switch(type) {
    case Rml::Core::Log::Type::LT_ERROR:
    case Rml::Core::Log::Type::LT_ASSERT:
    case Rml::Core::Log::Type::LT_WARNING:
      warning("rmlui: %s", message.c_str());
      break;
    case Rml::Core::Log::Type::LT_INFO:
#if L_DBG
    case Rml::Core::Log::Type::LT_DEBUG:
#endif
      log("rmlui: %s", message.c_str());
      break;
    default: break;
  }
  return true;
}
