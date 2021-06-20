#pragma once

#include "rmlui.h"

class RmlUiSystem : public Rml::SystemInterface {
  virtual double GetElapsedTime() override;
  virtual bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;
};
