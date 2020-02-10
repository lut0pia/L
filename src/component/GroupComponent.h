#pragma once

#include "Component.h"
#include "../engine/Resource.h"

namespace L {
  class GroupComponent : public TComponent<GroupComponent,
    ComponentFlag::Update> {
  protected:
    Array<Handle<Entity>> _entities;
    Resource<ScriptFunction> _level_script;
    Date _level_script_time = 0;

  public:
    ~GroupComponent();
    void update();
    static void script_registration();

    void link(Handle<Entity>);
    void unlink(Handle<Entity>);
    void unlink_all();

    inline const Array<Handle<Entity>>& entities() const { return _entities; }
    inline void level_script(const char* filepath) { _level_script = filepath; }
  };
}
