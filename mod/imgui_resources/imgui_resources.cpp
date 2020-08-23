#include <L/src/engine/Engine.h>
#include <L/src/engine/Resource.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Color.h>

#include <imgui.h>

using namespace L;

#if !L_RLS

static bool opened = false;

static Table<Symbol, Color> type_colors;
static uint32_t state_mask = (1 << ResourceSlot::Loading) | (1 << ResourceSlot::Failed);

static void imgui_resources_update() {
  if(ImGui::BeginMainMenuBar()) {
    if(ImGui::BeginMenu("Window")) {
      ImGui::MenuItem("Resources", "", &opened);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if(!opened) {
    return;
  }

  if(!ImGui::Begin("Resources", &opened)) {
    ImGui::End();
    return;
  }

  Array<ResourceSlot*> slots = ResourceSlot::slots();

  if(ImGui::BeginPopup("State Filter")) {
    ImGui::CheckboxFlags("Unloaded", &state_mask, 1 << ResourceSlot::Unloaded);
    ImGui::CheckboxFlags("Loading", &state_mask, 1 << ResourceSlot::Loading);
    ImGui::CheckboxFlags("Loaded", &state_mask, 1 << ResourceSlot::Loaded);
    ImGui::CheckboxFlags("Failed", &state_mask, 1 << ResourceSlot::Failed);
    ImGui::EndPopup();
  }

  ImGui::Columns(3, "Columns");

  ImGui::SetColumnWidth(0, 128);
  ImGui::SetColumnWidth(1, ImGui::GetWindowContentRegionWidth() - 128 - 64);
  ImGui::SetColumnWidth(2, 64);

  ImGui::Separator();

  ImGui::Text("Type");
  ImGui::NextColumn();

  ImGui::Text("Id");
  ImGui::NextColumn();

  if(ImGui::Button("State")) {
    ImGui::OpenPopup("State Filter");
  }
  ImGui::NextColumn();

  ImGui::Separator();

  for(ResourceSlot* slot : slots) {
    if(((1 << slot->state) & state_mask) == 0) {
      continue;
    }

    bool type_color_created;
    Color* type_color = type_colors.find_or_create(slot->type, &type_color_created);
    if(type_color_created) {
      *type_color = Color::from_index(type_colors.count() - 1);
    }
    const ImColor im_type_color = ImColor(type_color->rf(), type_color->gf(), type_color->bf());
    ImColor state_color = ImColor(1.f, 1.f, 1.f);
    const char* state = "Unknown";
    switch(slot->state) {
      case ResourceSlot::Unloaded: state = "Unloaded"; state_color = ImColor(0.5f, 0.5f, 0.5f);  break;
      case ResourceSlot::Loading: state = "Loading"; state_color = ImColor(1.f, 0.5f, 0.f);  break;
      case ResourceSlot::Loaded: state = "Loaded"; state_color = ImColor(0.f, 1.f, 0.f); break;
      case ResourceSlot::Failed: state = "Failed"; state_color = ImColor(1.f, 0.f, 0.f); break;
    }

    ImGui::TextColored(im_type_color, "%s", (const char*)slot->type);
    ImGui::NextColumn();

    ImGui::Text("%s", (const char*)slot->id);
    ImGui::NextColumn();

    ImGui::TextColored(state_color, "%s", state);
    ImGui::NextColumn();
  }
  ImGui::Columns(1);

  ImGui::End();
}
#endif

void imgui_resources_module_init() {
#if !L_RLS
  Engine::add_update(imgui_resources_update);
#endif
}

