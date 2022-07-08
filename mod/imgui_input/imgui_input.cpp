#include <L/src/engine/Engine.h>
#include <L/src/input/InputContext.h>

#include <imgui.h>
#include <imgui_integration.h>

using namespace L;

static void imgui_input_update() {
  if(!imgui_begin_toggleable_window("Input")) {
    return;
  }

  const Array<Handle<InputContext>>& contexts = InputContext::contexts();

  for(Handle<InputContext> context_handle : contexts) {
    InputContext* context = context_handle;
    if(context == nullptr) {
      continue;
    }

    if(ImGui::CollapsingHeader(context->get_name())) {
      const char* block_mode_string = "None";
      switch(context->get_block_mode()) {
        case InputBlockMode::Used: block_mode_string = "Used"; break;
        case InputBlockMode::All: block_mode_string = "All"; break;
        default: break;
      }

      ImGui::Text("Block mode: %s", block_mode_string);

      const auto& inputs = context->get_inputs();
      if(inputs.count() > 0) {
        ImGui::Columns(2, "Inputs");
        ImGui::Text("Name");
        ImGui::NextColumn();
        ImGui::Text("Value");
        ImGui::NextColumn();
        ImGui::Separator();

        for(const auto& input : context->get_inputs()) {
          ImGui::Text("%s", (const char*)input.key());
          ImGui::NextColumn();
          ImGui::Text("%f", input.value());
          ImGui::NextColumn();
        }
        ImGui::Columns(1);
      }
    }
  }

  imgui_end_toggleable_window();
}

void imgui_input_module_init() {
  Engine::add_update(imgui_input_update);
}

