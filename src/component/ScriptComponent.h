#pragma once

#include "Component.h"
#include "../input/Device.h"
#include "../script/ScriptContext.h"
#include "../engine/Resource.h"

namespace L {
  class ScriptComponent : public TComponent<ScriptComponent,
    ComponentFlag::Update | ComponentFlag::LateUpdate> {
  protected:
    ScriptContext _context;
    Resource<ScriptFunction> _script;
    bool _started : 1;
  public:
    ScriptComponent() : _started(false) {}
    virtual void update_components() override;
    static void script_registration();

    void load(const char* filename);
    void start();
    void update();
    void late_update();
    void event(const Ref<Table<Var, Var>>&);
  };
}
