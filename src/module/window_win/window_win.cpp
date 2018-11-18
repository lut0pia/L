#define VK_USE_PLATFORM_WIN32_KHR

#include <L/src/engine/Engine.h>
#include <L/src/parallelism/TaskSystem.h>
#include <L/src/rendering/Vulkan.h>
#include <L/src/system/Window.h>

#include <windows.h>
#include <windowsx.h>

using namespace L;

class WinAPIWindow : public Window {
protected:
  static WinAPIWindow* _win_instance;
  HWND hWND;

public:
  inline WinAPIWindow() {
    L_ASSERT(_win_instance==nullptr);
    _win_instance = this;
  }
  inline WinAPIWindow::~WinAPIWindow() {
    L_ASSERT(_win_instance!=nullptr);
    _win_instance = nullptr;
  }
  static LRESULT CALLBACK MainWndProc(HWND hwnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam) {
    Window::Event e {};
    switch(uMsg) {
      case WM_CREATE:
        return 0;
        break;
      case WM_DESTROY:
        PostQuitMessage(0);
        _win_instance->_opened = false;
        return 0;
        break;
      case WM_MOUSEMOVE:
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
      case WM_MBUTTONDOWN:
      case WM_MBUTTONUP:
        _win_instance->_cursor_x = GET_X_LPARAM(lParam);
        _win_instance->_cursor_y = GET_Y_LPARAM(lParam);
        return 0;
      case WM_SETCURSOR:
        SetCursor((_win_instance->_flags & Window::nocursor) ? nullptr : LoadCursor(nullptr, IDC_ARROW));
        break;
      case WM_SIZE: // The size of the window has changed
        e.type = Event::Type::Resize;
        e.x = _win_instance->_width = LOWORD(lParam);
        e.y = _win_instance->_height = HIWORD(lParam);
        break;
      default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
        break;
    }
    if(e.type!=Event::Type::None) {
      _win_instance->_events.push(e);
    }
    return 0;
  }
  static void update() {
    L_SCOPE_MARKER("Window update");
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
  }

  void open(const char* title, uint32_t width, uint32_t height, uint32_t flags) override {
    L_SCOPE_MARKER("Window::open");
    if(opened()) return;
    _width = width;
    _height = height;
    _flags = flags;


    { // Register class
      WNDCLASS wc;
      wc.style = CS_OWNDC;
      wc.lpfnWndProc = MainWndProc;
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;
      wc.hInstance = GetModuleHandle(nullptr);
      wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(101));
      wc.hCursor = (_flags & Window::nocursor) ? nullptr : LoadCursor(nullptr, IDC_ARROW);
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
    _opened = true;
  }
  void close() override {
    L_ASSERT(_opened);
    DestroyWindow(hWND);
    _opened = false;
  }

  void title(const char* str) override {
    L_ASSERT(opened());
    SetWindowText(hWND, str);
  }
  void resize(uint32_t width, uint32_t height) override {
    L_ASSERT(opened());
    SetWindowPos(hWND, HWND_NOTOPMOST, 0, 0, width, height, SWP_NOMOVE|SWP_NOZORDER);
  }
  void create_vulkan_surface(VkInstance instance, VkSurfaceKHR* surface) override {
    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hwnd = hWND;
    create_info.hinstance = GetModuleHandle(nullptr);
    L_VK_CHECKED(vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, surface));
  }
  const char* extra_vulkan_extension() override {
    return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
  }
};

WinAPIWindow* WinAPIWindow::_win_instance;

void window_win_module_init() {
  Memory::new_type<WinAPIWindow>();
  Engine::add_parallel_update([]() {
    WinAPIWindow::update();
  });
}