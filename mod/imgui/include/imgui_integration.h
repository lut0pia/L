#pragma once

#include <imgui.h>

bool imgui_begin_main_menu_bar();
void imgui_end_main_menu_bar();

bool imgui_begin_toggleable_window(const char* path);
void imgui_end_toggleable_window();
