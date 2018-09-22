#include "Window.h"

#include "../container/Queue.h"
#include "Device.h"
#include "../dev/profiling.h"
#include "../macros.h"
#include "../stream/CFileStream.h"
#include "../text/encoding.h"

#include <windows.h>
#include <windowsx.h>
#include "../rendering/Vulkan.h"

using namespace L;

namespace L {
  extern Queue<64, Window::Event> window_events;
  extern int window_width, window_height, window_flags, mouse_x, mouse_y;
}

HWND hWND;
LRESULT CALLBACK MainWndProc(HWND hwnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam) {
  Window::Event e;
  switch(uMsg) {
    case WM_CREATE:
      return 0;
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      hWND = 0;
      return 0;
      break;
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
      mouse_x = GET_X_LPARAM(lParam);
      mouse_y = GET_Y_LPARAM(lParam);
      return 0;
    case WM_SETCURSOR:
      SetCursor((window_flags& Window::nocursor) ? nullptr : LoadCursor(nullptr, IDC_ARROW));
      break;
    case WM_SIZE: // The size of the window has changed
      e.type = Window::Event::Resize;
      e.x = LOWORD(lParam);
      e.y = HIWORD(lParam);
      break;
    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
      break;
  }
  if(e.type!=Window::Event::None) {
    window_events.push(e);
  }
  return 0;
}

void Window::open(const char* title, uint32_t width, uint32_t height, uint32_t flags) {
  L_SCOPE_MARKER("Window::open");
  if(opened()) return;
  window_width = width;
  window_height = height;
  window_flags = flags;


  { // Register class
    WNDCLASS wc;
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(101));
    wc.hCursor = (window_flags & Window::nocursor) ? nullptr : LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = "LWC";
    RegisterClass(&wc);
  }

  // Create window style
  DWORD wStyle = ((flags & borderless) ? (WS_POPUP) : (WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU))
    | ((flags & resizable) ? (WS_MAXIMIZEBOX) : 0)
    | ((flags & resizable) ? (WS_SIZEBOX) : 0)
    | WS_VISIBLE;

  // Find out needed window size for wanted client area size
  RECT rect = {0,0,(int)width,(int)height};
  AdjustWindowRect(&rect, wStyle, false);
  width = rect.right-rect.left;
  height = rect.bottom-rect.top;

  // Create window
  HINSTANCE hInstance(GetModuleHandle(nullptr));
  hWND = CreateWindow("LWC", title, wStyle,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height,
    nullptr, nullptr, hInstance, nullptr);

  Vulkan::init();
}
void Window::close() {
  L_ASSERT(opened());
  DestroyWindow(hWND);
  hWND = 0;
}
bool Window::opened() { return hWND!=0; }
bool Window::loop() {
  L_SCOPE_MARKER("Window::loop");
  L_SCOPE_THREAD_MASK(1);
  MSG msg;
  // We need not to remove WP_INPUT messages because they may be consumed by a device module
  while(opened() && PeekMessage(&msg, nullptr, 0, WM_INPUT - 1, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  while(opened() && PeekMessage(&msg, nullptr, WM_INPUT + 1, (UINT)-1, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return opened();
}

void Window::title(const char* str) {
  L_ASSERT(opened());
  SetWindowText(hWND, str);
}
void Window::resize(uint32_t width, uint32_t height) {
  L_ASSERT(opened());
  SetWindowPos(hWND, HWND_NOTOPMOST, 0, 0, width, height, SWP_NOMOVE|SWP_NOZORDER);
}
