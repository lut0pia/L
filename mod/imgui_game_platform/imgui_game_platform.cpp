#include <L/src/engine/Engine.h>
#include <L/src/engine/GamePlatform.h>
#include <L/src/stream/StringStream.h>
#include <imgui_integration.h>

using namespace L;

static void imgui_game_platform_update() {
  if(!imgui_begin_toggleable_window("Game Platform")) {
    return;
  }

  for(Handle<GamePlatform> game_platform : GamePlatform::get_game_platforms()) {
    if(!game_platform.is_valid()) {
      continue;
    }

    if(ImGui::CollapsingHeader(game_platform->get_name(), ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("Player name: %s", game_platform->get_player_name().begin());
      if(ImGui::BeginTable("Achievements", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Sortable)) {

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 60);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        Array<GameAchievement> achievements = game_platform->get_achievements();

        static ImGuiTableSortSpecs* sort_specs;
        sort_specs = ImGui::TableGetSortSpecs();
        if(sort_specs != nullptr) {
          qsort(achievements.begin(), achievements.size(), sizeof(achievements[0]),
            [](void const* lhs, void const* rhs) -> int {
              const GameAchievement* a = (const GameAchievement*)lhs;
              const GameAchievement* b = (const GameAchievement*)rhs;
              for(int n = 0; n < sort_specs->SpecsCount; n++) {
                const ImGuiTableColumnSortSpecs* sort_spec = &sort_specs->Specs[n];
                int delta = 0;
                switch(sort_spec->ColumnIndex) {
                  case 0: delta = strcmp(a->name, b->name); break;
                  case 1:
                    if(a->is_unlocked && b->is_unlocked) {
                      delta = static_cast<int>(a->unlock_time.get_time() - b->unlock_time.get_time());
                    } else {
                      delta = static_cast<int>(a->is_unlocked - b->is_unlocked);
                    }
                    break;
                  case 2: delta = static_cast<int>(a->is_unlocked - b->is_unlocked); break;
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

        for(const GameAchievement& achievement : achievements) {
          ImGui::TableNextRow();
          ImGui::PushID(achievement.name);

          ImGui::TableSetColumnIndex(0);
          ImGui::Text("%s", achievement.name.begin());

          ImGui::TableSetColumnIndex(1);
          if(achievement.is_unlocked) {
            ImGui::Text("Achieved");
            if(ImGui::IsItemHovered()) {
              ImGui::SetTooltip("%s", to_string(achievement.unlock_time).begin());
            }
          } else {
            ImGui::Text("Pending");
          }

          ImGui::TableSetColumnIndex(2);
          if(!achievement.is_unlocked) {
            if(ImGui::SmallButton("Unlock")) {
              game_platform->unlock_achievement(achievement.name);
            }
          } else {
            if(ImGui::SmallButton("Clear")) {
              game_platform->clear_achievement(achievement.name);
            }
          }

          ImGui::PopID();
        }
        ImGui::EndTable();
      }
    }
  }

  imgui_end_toggleable_window();
}

void imgui_game_platform_module_init() {
  Engine::add_update(imgui_game_platform_update);
}
