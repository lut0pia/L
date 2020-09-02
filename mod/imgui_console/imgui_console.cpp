#include <L/src/engine/Engine.h>
#include <L/src/engine/Resource.h>
#include <L/src/engine/Resource.inl>

#include <cctype>
#include <imgui.h>

using namespace L;

static bool opened = false;
static char input_buffer[512];
static int32_t scroll_to_bottom = 0, history_pos = -1;
static Array<String> history, items, commands, candidates;
static ScriptContext script_context;
static Symbol script_language;

static int text_edit_callback(ImGuiInputTextCallbackData* data) {
  switch(data->EventFlag) {
    case ImGuiInputTextFlags_CallbackCompletion:
    {
      // Locate beginning of current word
      const char* word_end = data->Buf + data->CursorPos;
      const char* word_start = word_end;
      while(word_start > data->Buf) {
        const char c = word_start[-1];
        if(c == ' ' || c == '\t' || c == ',' || c == ';' || c == '.') {
          break;
        }
        word_start--;
      }

      // Build a list of candidates
      candidates.clear();
      for(uintptr_t i = 0; i < commands.size(); i++)
        if(strncmp(commands[i], word_start, (int)(word_end - word_start)) == 0)
          candidates.push(commands[i]);

      if(candidates.size() == 0) {
        // No match
        //AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
      } else if(candidates.size() == 1) {
        // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
        data->InsertChars(data->CursorPos, candidates[0]);
        data->InsertChars(data->CursorPos, " ");
      } else {
        // Multiple matches. Complete as much as we can..
        // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
        int match_len = (int)(word_end - word_start);
        for(;;) {
          int c = 0;
          bool all_candidates_matches = true;
          for(uintptr_t i = 0; i < candidates.size() && all_candidates_matches; i++) {
            if(i == 0) {
              c = toupper(candidates[i][match_len]);
            } else if(c == 0 || c != toupper(candidates[i][match_len])) {
              all_candidates_matches = false;
            }
          }
          if(!all_candidates_matches) {
            break;
          }
          match_len++;
        }

        if(match_len > 0) {
          data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
          data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
        }

        items.push("Candidates:");
        for(const String& candidate : candidates) {
          items.push("- " + candidate);
        }
      }

      break;
    }
    case ImGuiInputTextFlags_CallbackHistory:
    {
      const int32_t prev_history_pos = history_pos;
      if(data->EventKey == ImGuiKey_UpArrow) {
        if(history_pos == -1) {
          history_pos = int32_t(history.size() - 1);
        } else if(history_pos > 0) {
          history_pos--;
        }
      } else if(data->EventKey == ImGuiKey_DownArrow) {
        if(history_pos != -1) {
          if(++history_pos >= int32_t(history.size())) {
            history_pos = -1;
          }
        }
      }

      // A better implementation would preserve the data on the current input line along with cursor position.
      if(prev_history_pos != history_pos) {
        data->DeleteChars(0, data->BufTextLen);
        if(history_pos >= 0) {
          data->InsertChars(0, history[history_pos]);
        }
      }
      break;
    }
  }
  return 0;
}

static void imgui_console_update() {
  ImGuiIO& io = ImGui::GetIO();
  if(io.KeyCtrl && ImGui::IsKeyPressed(int(Device::Button::F12))) {
    opened = !opened;
  }

  if(!opened) {
    return;
  }

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y / 2));
  ImGui::SetNextWindowFocus();
  if(!ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove)) {
    ImGui::End();
    return;
  }

  const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
  ImGui::BeginChild("Log", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
  for(uintptr_t i = 0; i < items.size(); i++) {
    const String& item = items[i];
    ImVec4 color;
    bool has_color = false;

    if(strstr(item, "> ")) {
      color = ImVec4(0.7f, 0.7f, 1.0f, 1.0f);
      has_color = true;
    }

    if(has_color) {
      ImGui::PushStyleColor(ImGuiCol_Text, color);
    }
    ImGui::TextUnformatted(item);
    if(has_color) {
      ImGui::PopStyleColor();
    }
  }
  if(scroll_to_bottom > 0) {
    ImGui::SetScrollHereY(1.0f);
    scroll_to_bottom--;
  }
  ImGui::EndChild();

  ImGui::Separator();

  // Text input
  ImGui::SetNextItemWidth(-1.f);
  ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
  if(ImGui::InputText("", input_buffer, IM_ARRAYSIZE(input_buffer), input_text_flags, &text_edit_callback) && *input_buffer) {
    items.push(String("> ") + input_buffer);
    history.push(input_buffer);

    ResourceSlot res_slot(type_name<ScriptFunction>(), "imgui_console");
    res_slot.source_buffer = Buffer(input_buffer, strlen(input_buffer));
    res_slot.ext = script_language;

    Ref<ScriptFunction> script_function = ref<ScriptFunction>();
    if(ResourceLoading<ScriptFunction>::load_internal(res_slot, *script_function)) {
      const Var result = script_context.execute(script_function);

      StringStream string_stream;
      string_stream << result;
      items.push(string_stream.string());
    } else {
      items.push("Could not compile!");
    }
    strcpy(input_buffer, "");
    scroll_to_bottom = 2;
  }

  ImGui::SetItemDefaultFocus();
  ImGui::SetKeyboardFocusHere(-1); // Auto focus text input always

  ImGui::End();
}

void imgui_console_module_init() {
  script_language = "ls";
  Engine::add_update(imgui_console_update);
}
