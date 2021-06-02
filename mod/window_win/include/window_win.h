#pragma once

#include <windows.h>

#include <L/src/rendering/Renderer.h>

extern L::Symbol win32_window_type;

struct Win32WindowData : public L::GenericWindowData {
  HMODULE module;
  HWND window;
};

