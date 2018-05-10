#include "Window.h"

#include "../dev/profiling.h"
#include "../rendering/GL.h"
#include "../macros.h"
#include "../stream/CFileStream.h"
#include "../text/encoding.h"
#include "Device.h"

#include <GL/wglext.h>
#include <windows.h>

using namespace L;

bool* buttonstate_p;
Queue<512,Window::Event>* events_p;
Vector2i* mousePos_p;
int* flags_p;

HWND hWND;
HDC hDC;
HGLRC hRC;
HBITMAP hBITMAP = 0;
LRESULT CALLBACK MainWndProc(HWND hwnd,uint32_t uMsg,WPARAM wParam,LPARAM lParam) {
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
    case WM_KEYDOWN:
    case WM_KEYUP: // Key pressed
      e.type = (uMsg==WM_KEYDOWN) ? Window::Event::ButtonDown : Window::Event::ButtonUp;
      switch(wParam) {
#define MAP(a,b) case a: e.button = Window::Event::b; break;
        MAP(VK_BACK,Backspace)
          MAP(VK_TAB,Tab)
          MAP(VK_RETURN,Enter)
          MAP(VK_SHIFT,Shift)
          MAP(VK_CONTROL,Ctrl)
          MAP(VK_MENU,Alt)
          MAP(VK_PAUSE,Pause)
          MAP(VK_CAPITAL,Caps)
          MAP(VK_ESCAPE,Escape)
          MAP(VK_SPACE,Space)
          MAP(VK_LEFT,Left) MAP(VK_RIGHT,Right) MAP(VK_UP,Up) MAP(VK_DOWN,Down)
          MAP(VK_NUMLOCK,NumLock)
          MAP(VK_NUMPAD1,Num1) MAP(VK_NUMPAD2,Num2) MAP(VK_NUMPAD3,Num3)
          MAP(VK_NUMPAD4,Num4) MAP(VK_NUMPAD5,Num5) MAP(VK_NUMPAD6,Num6)
          MAP(VK_NUMPAD7,Num7) MAP(VK_NUMPAD8,Num8) MAP(VK_NUMPAD9,Num9)
          MAP(VK_NUMPAD0,Num0)
          MAP(VK_F1,F1) MAP(VK_F2,F2) MAP(VK_F3,F3) MAP(VK_F4,F4)
          MAP(VK_F5,F5) MAP(VK_F6,F6) MAP(VK_F7,F7) MAP(VK_F8,F8)
          MAP(VK_F9,F9) MAP(VK_F10,F10) MAP(VK_F11,F11) MAP(VK_F12,F12)
#undef MAP
        default:
          if((wParam>='0' && wParam<='9') || (wParam>='A' && wParam<='Z'))
            e.button = (Window::Event::Button)wParam;
          else return 0;
      }
      break;
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
      if(uMsg==WM_MOUSEMOVE)
        e.type = Window::Event::MouseMove;
      else {
        if(uMsg==WM_LBUTTONDOWN || uMsg==WM_RBUTTONDOWN || uMsg==WM_MBUTTONDOWN)
          e.type = Window::Event::ButtonDown;
        else e.type = Window::Event::ButtonUp;
        switch(uMsg) {
          case WM_LBUTTONDOWN:
          case WM_LBUTTONUP:
            e.button = Window::Event::LeftButton;
            break;
          case WM_RBUTTONDOWN:
          case WM_RBUTTONUP:
            e.button = Window::Event::RightButton;
            break;
          case WM_MBUTTONDOWN:
          case WM_MBUTTONUP:
            e.button = Window::Event::MiddleButton;
            break;
        }
      }
      e.x = GET_X_LPARAM(lParam);
      e.y = GET_Y_LPARAM(lParam);
      e.x -= mousePos_p->x();
      e.y -= mousePos_p->y();
      if(*flags_p&Window::loopcursor) {
        POINT p = {128, 128};
        ClientToScreen(hWND, &p);
        SetCursorPos(p.x, p.y);
        *mousePos_p = Vector2i(128, 128);
      } else {
        *mousePos_p += Vector2i(e.x, e.y);
      }
      break;
    case WM_MOUSEWHEEL:
      e.type = Window::Event::MouseWheel;
      e.y = GET_WHEEL_DELTA_WPARAM(wParam);
      break;
    case WM_SIZE: // The size of the window has changed
      e.type = Window::Event::Resize;
      e.x = LOWORD(lParam);
      e.y = HIWORD(lParam);
      break;
    case WM_CHAR:
      if(wParam != '\b' && wParam != '\r') {
        e.type = Window::Event::Text;
        strcpy(e.text, utf16_to_utf8(wParam));
      } else return 0;
      break;
    case WM_SETCURSOR:
      SetCursor((*flags_p & Window::nocursor) ? nullptr : LoadCursor(nullptr,IDC_ARROW));
      break;
    case WM_INPUT:
    {
      BYTE buffer[1024];
      UINT dwSize(sizeof(buffer));
      if(GetRawInputData((HRAWINPUT)lParam,RID_INPUT,buffer,&dwSize,sizeof(RAWINPUTHEADER))>1024)
        error("RawInput buffer too small, panic!");
      const RAWINPUT* raw((RAWINPUT*)buffer);
      Device::processReport(raw->header.hDevice,raw->data.hid.bRawData,raw->data.hid.dwSizeHid);
      return DefWindowProc(hwnd,uMsg,wParam,lParam);
      break;
    }
    default:
      return DefWindowProc(hwnd,uMsg,wParam,lParam);
      break;
  }
  if(e.type!=Window::Event::None){
    if(e.type==Window::Event::ButtonDown)
      buttonstate_p[e.button] = true;
    else if(e.type==Window::Event::ButtonUp)
      buttonstate_p[e.button] = false;
    events_p->push(e);
  }
  return 0;
}

void registerClass(){
  L_ONCE;
  WNDCLASS wc;
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = MainWndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hIcon = LoadIcon(nullptr,IDI_APPLICATION);
  wc.hCursor = (*flags_p & Window::nocursor) ? nullptr : LoadCursor(nullptr,IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = nullptr;
  wc.lpszClassName = "LWC";
  RegisterClass(&wc);
}

PIXELFORMATDESCRIPTOR pfd;
PIXELFORMATDESCRIPTOR* initPFD(){
  L_DO_ONCE{
    ZeroMemory(&pfd,sizeof(pfd)); // Initialize pixel format descriptor
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 0;
    pfd.cStencilBits = 0;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.iLayerType = PFD_MAIN_PLANE;
  }
  return &pfd;
}

void Window::open(const char* title, int width, int height, int flags) {
  L_SCOPE_MARKER("Window::open");
  buttonstate_p = _buttonstate;
  events_p = &_events;
  mousePos_p = &_mousePos;
  flags_p = &_flags;
  if(opened()) return;
  _width = width;
  _height = height;
  _flags = flags;

  HINSTANCE hInstance(GetModuleHandle(nullptr));

  registerClass();

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
  hWND = CreateWindow("LWC", title, wStyle,
                      CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                      nullptr, nullptr, hInstance, nullptr);

  hDC = GetDC(hWND);

  int pf = ChoosePixelFormat(hDC, initPFD());
  if(!pf)
    error("ChoosePixelFormat failed during GLEW initialization");

  if(!SetPixelFormat(hDC, pf, &pfd))
    error("SetPixelFormat failed during GLEW initialization");

  HGLRC hRCFake = wglCreateContext(hDC);
  wglMakeCurrent(hDC, hRCFake);

  _mousePos = Vector2i(width/2, height/2);
  SetCursorPos(width/2, height/2);

  int iContextAttribs[] =
  {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 0,
#ifdef L_DEBUG
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0 // End of attributes list
  };

  static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB(
    PFNWGLCREATECONTEXTATTRIBSARBPROC(wglGetProcAddress("wglCreateContextAttribsARB")));
  if(!(hRC = wglCreateContextAttribsARB(hDC, 0, iContextAttribs)))
    error("wglCreateContextAttribsARB failed");

  wglMakeCurrent(hDC, hRC);
  wglDeleteContext(hRCFake);

  GL::init();
}
void Window::close() {
  L_ASSERT(opened());
  DestroyWindow(hWND);
  hWND = 0;
}
bool Window::opened() { return hWND!=0; }
bool Window::loop() {
  L_SCOPE_MARKER("Window::loop");
  MSG msg;
  while(opened() && PeekMessage(&msg,nullptr,0,0,PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return opened();
}
void Window::swapBuffers() {
  L_ASSERT(opened());
  SwapBuffers(hDC);
}
void Window::draw(const Bitmap& bmp) {
  L_ASSERT(opened());
  HBITMAP hbmp = CreateBitmap(bmp.width(),bmp.height(),1,32,&bmp(0,0)),htmp;
  HDC hMemDC = CreateCompatibleDC(hDC);
  htmp = (HBITMAP)SelectObject(hMemDC,hbmp);
  BitBlt(hDC,0,0,bmp.width(),bmp.height(),hMemDC,0,0,SRCCOPY);
  SelectObject(hMemDC,htmp);
  DeleteObject(hbmp);
  DeleteDC(hMemDC);
}

void Window::title(const char* str) {
  L_ASSERT(opened());
  SetWindowText(hWND,str);
}
void Window::resize(int width,int height) {
  L_ASSERT(opened());
  SetWindowPos(hWND,HWND_NOTOPMOST,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER);
}
void Window::mousePosition(const Vector2i& p){
  SetCursorPos(p.x(),p.y());
}