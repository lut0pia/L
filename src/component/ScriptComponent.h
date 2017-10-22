#pragma once

#include "Component.h"
#include "../script/Context.h"
#include "../system/Device.h"

namespace L {
  class ScriptComponent : public Component {
    L_COMPONENT(ScriptComponent)
      L_COMPONENT_HAS_UPDATE(ScriptComponent)
      L_COMPONENT_HAS_LATE_UPDATE(ScriptComponent)
      L_COMPONENT_HAS_GUI(ScriptComponent)
      L_COMPONENT_HAS_WIN_EVENT(ScriptComponent)
      L_COMPONENT_HAS_DEV_EVENT(ScriptComponent)
  protected:
    Script::Context _context;
    String _script_path;
    bool _started : 1;
  public:
    ScriptComponent() : _started(false) {}
    virtual void update_components() override;
    virtual Map<Symbol, Var> pack() const override;
    virtual void unpack(const Map<Symbol, Var>&) override;

    void load(const char* filename);
    void start();
    void update();
    void late_update();
    void event(const Device::Event&);
    void event(const Window::Event&);
    void event(const Ref<Table<Var, Var>>&);
    void gui(const Camera&);
    static void init();
  };
}
