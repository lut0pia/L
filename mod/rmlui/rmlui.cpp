#include <L/src/engine/Engine.h>

#include "RmlUiFile.h"
#include "RmlUiFontEngine.h"
#include "RmlUiRender.h"
#include "RmlUiSystem.h"

using namespace L;

static RmlUiFile file_implementation;
static RmlUiFontEngine font_engine_implementation;
static RmlUiRender render_implementation;
static RmlUiSystem system_implementation;

static void rmlui_update() {
#if !L_RLS
  if(file_implementation.HasOutOfDateResources()) {
    ComponentPool<RmlUiComponent>::iterate(
      [&](RmlUiComponent& component) {
        component.refresh();
      });
  }
#endif
  render_implementation.Update();
}

static void rmlui_gui(const class Camera& camera) {
  render_implementation.Render(camera);
}

void rmlui_module_init() {
  Rml::Core::SetFileInterface(&file_implementation);
  Rml::Core::SetFontEngineInterface(&font_engine_implementation);
  Rml::Core::SetRenderInterface(&render_implementation);
  Rml::Core::SetSystemInterface(&system_implementation);

  if(!Rml::Core::Initialise()) {
    warning("rmlui: Could not initialize");
    return;
  }

  Engine::register_component<RmlUiComponent>();

  // Defer init because we need resources and task system up
  Engine::add_deferred_action(Engine::DeferredAction {
    [](void*) {
      render_implementation.Init();
      Engine::add_update(rmlui_update);
      Engine::add_gui(rmlui_gui);
  }});

  Engine::add_shutdown(
    []() {
      render_implementation.Shutdown();
    });
}
