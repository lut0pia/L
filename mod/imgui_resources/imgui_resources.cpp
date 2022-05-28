#include <L/src/engine/Engine.h>
#include <L/src/engine/Resource.h>
#include <L/src/math/math.h>
#include <L/src/rendering/Color.h>
#include <L/src/text/format.h>
#include <imgui_integration.h>

#include <L/src/engine/Resource.inl>

using namespace L;

#if !L_RLS

static bool opened = false;

static Table<Symbol, Color> type_colors;
static uint32_t state_mask = (1 << uint32_t(ResourceState::Loading)) | (1 << uint32_t(ResourceState::Failed));

static void imgui_resources_update() {
  if(imgui_begin_main_menu_bar()) {
    if(ImGui::BeginMenu("Window")) {
      ImGui::MenuItem("Resources", "", &opened);
      ImGui::EndMenu();
    }
    imgui_end_main_menu_bar();
  }

  if(!opened) {
    return;
  }

  if(!ImGui::Begin("Resources", &opened)) {
    ImGui::End();
    return;
  }

  Array<ResourceSlot*> slots = ResourceSlot::slots();

  if(ImGui::BeginPopup("State Filter Popup")) {
    ImGui::CheckboxFlags("Unloaded", &state_mask, 1 << uint32_t(ResourceState::Unloaded));
    ImGui::CheckboxFlags("Loading", &state_mask, 1 << uint32_t(ResourceState::Loading));
    ImGui::CheckboxFlags("Loaded", &state_mask, 1 << uint32_t(ResourceState::Loaded));
    ImGui::CheckboxFlags("Failed", &state_mask, 1 << uint32_t(ResourceState::Failed));
    ImGui::EndPopup();
  }

  if(ImGui::Button("State Filter")) {
    ImGui::OpenPopup("State Filter Popup");
  }

  if(!ImGui::BeginTable("Columns", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Sortable)) {
    ImGui::End();
    return;
  }

  ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100);
  ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthStretch);
  ImGui::TableSetupColumn("CPU", ImGuiTableColumnFlags_WidthFixed, 50);
  ImGui::TableSetupColumn("GPU", ImGuiTableColumnFlags_WidthFixed, 50);
  ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, 60);
  ImGui::TableSetupScrollFreeze(0, 1);
  ImGui::TableHeadersRow();

  // Sort slots according to ImGui sort specs
  static ImGuiTableSortSpecs* sort_specs;
  sort_specs = ImGui::TableGetSortSpecs();
  if(sort_specs != nullptr) {
    qsort(slots.begin(), slots.size(), sizeof(slots[0]),
      [](void const* lhs, void const* rhs) -> int {
        const ResourceSlot* a = *(const ResourceSlot**)lhs;
        const ResourceSlot* b = *(const ResourceSlot**)rhs;
        for(int n = 0; n < sort_specs->SpecsCount; n++) {
          const ImGuiTableColumnSortSpecs* sort_spec = &sort_specs->Specs[n];
          int delta = 0;
          switch(sort_spec->ColumnIndex) {
            case 0: delta = strcmp(a->type, b->type); break;
            case 1: delta = strcmp(a->id, b->id); break; // TODO: Should be case-insensitive
            case 2: delta = static_cast<int>(a->cpu_size - b->cpu_size); break;
            case 3: delta = static_cast<int>(a->gpu_size - b->gpu_size); break;
            case 4: delta = static_cast<int>(a->state.load()) - static_cast<int>(b->state.load()); break;
            default: IM_ASSERT(0); break;
          }
          if(delta > 0) {
            return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
          } else if(delta < 0) {
            return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
          }
        }

        return static_cast<int>(a - b);
      });
  }

  for(ResourceSlot* slot : slots) {
    if(((1 << uint32_t(slot->state.load())) & state_mask) == 0) {
      continue;
    }

    ImGui::TableNextRow();

    bool type_color_created;
    Color* type_color = type_colors.find_or_create(slot->type, &type_color_created);
    if(type_color_created) {
      *type_color = Color::from_index(type_colors.count() - 1);
    }
    const ImColor im_type_color = ImColor(type_color->rf(), type_color->gf(), type_color->bf());
    ImColor state_color = ImColor(1.f, 1.f, 1.f);
    const char* state = "Unknown";
    switch(slot->state) {
      case ResourceState::Unloaded:
        state = "Unloaded";
        state_color = ImColor(0.5f, 0.5f, 0.5f);
        break;
      case ResourceState::Loading:
        state = "Loading";
        state_color = ImColor(1.f, 0.5f, 0.f);
        break;
      case ResourceState::Loaded:
        state = "Loaded";
        state_color = ImColor(0.f, 1.f, 0.f);
        break;
      case ResourceState::Failed:
        state = "Failed";
        state_color = ImColor(1.f, 0.f, 0.f);
        break;
    }

    String type = String(slot->type).replace_all("L::", "");
    ImGui::TableSetColumnIndex(0);
    ImGui::TextColored(im_type_color, "%s", type.begin());

    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%s", (const char*)slot->id);

    ImGui::TableSetColumnIndex(2);
    if(slot->cpu_size > 0) {
      const String cpu_size = format_memory_amount(slot->cpu_size);
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 50 - ImGui::CalcTextSize(cpu_size.begin()).x);
      ImGui::Text("%s", cpu_size.begin());
    }

    ImGui::TableSetColumnIndex(3);
    if(slot->gpu_size > 0) {
      const String gpu_size = format_memory_amount(slot->gpu_size);
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 50 - ImGui::CalcTextSize(gpu_size.begin()).x);
      ImGui::Text("%s", gpu_size.begin());
    }

    ImGui::TableSetColumnIndex(4);
    ImGui::TextColored(state_color, "%s", state);
  }

  ImGui::EndTable();

  ImGui::End();
}
#endif

void imgui_resources_module_init() {
#if !L_RLS
  Engine::add_update(imgui_resources_update);
#endif
}
