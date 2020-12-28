#pragma once

#include "rmlui.h"

class RmlUiSystem : public Rml::Core::SystemInterface {
  virtual double GetElapsedTime() override;
  virtual bool LogMessage(Rml::Core::Log::Type type, const Rml::Core::String& message) override;
};
