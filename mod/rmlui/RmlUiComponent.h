#pragma once

#include <L/src/component/Component.h>
#include <L/src/input/InputContext.h>
#include <L/src/rendering/Material.h>

#include "rmlui.h"

class RmlUiComponent : public L::TComponent<RmlUiComponent> {
protected:
  struct EventListener : public Rml::Core::EventListener {
    RmlUiSafeElement element;
    L::Ref<L::ScriptFunction> function;
    L::ScriptContext script_context;

    virtual void ProcessEvent(Rml::Core::Event& event) override;
  };
  static L::Array<EventListener*> _event_listeners;
  L::Camera* _camera;
  Rml::Core::Context* _context;
  L::InputContext _input_context;

public:
  RmlUiComponent();
  ~RmlUiComponent();

  virtual void update_components() override;
  static void script_registration();

  void update();

  RmlUiSafeElement load_document(const char* path);

  inline Rml::Core::Context* get_context() { return _context; }

#if !L_RLS
  void refresh();
#endif
};

