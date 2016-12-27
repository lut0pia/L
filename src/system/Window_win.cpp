#include "Window.h"

#include "../gl/GL.h"
#include "../text/encoding.h"
#include "Device.h"
#include <windows.h>
#include <GL/wglew.h>

using namespace L;

bool* buttonstate;
StaticRing<512,Window::Event>* events;
Vector2i* mousePos;
int* flags;

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
      e.type = (uMsg==WM_KEYDOWN) ? Window::Event::BUTTONDOWN : Window::Event::BUTTONUP;
      switch(wParam) {
#define MAP(a,b) case a: e.button = Window::Event::b; break;
        MAP(VK_BACK,BACKSPACE)
          MAP(VK_TAB,TAB)
          MAP(VK_RETURN,ENTER)
          MAP(VK_SHIFT,SHIFT)
          MAP(VK_CONTROL,CTRL)
          MAP(VK_MENU,ALT)
          MAP(VK_PAUSE,PAUSE)
          MAP(VK_CAPITAL,CAPS)
          MAP(VK_ESCAPE,ESCAPE)
          MAP(VK_SPACE,SPACE)
          MAP(VK_LEFT,LEFT) MAP(VK_RIGHT,RIGHT) MAP(VK_UP,UP) MAP(VK_DOWN,DOWN)
          MAP(VK_NUMLOCK,NUMLOCK)
          MAP(VK_NUMPAD1,NUM1) MAP(VK_NUMPAD2,NUM2) MAP(VK_NUMPAD3,NUM3)
          MAP(VK_NUMPAD4,NUM4) MAP(VK_NUMPAD5,NUM5) MAP(VK_NUMPAD6,NUM6)
          MAP(VK_NUMPAD7,NUM7) MAP(VK_NUMPAD8,NUM8) MAP(VK_NUMPAD9,NUM9)
          MAP(VK_NUMPAD0,NUM0)
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
        e.type = Window::Event::MOUSEMOVE;
      else {
        if(uMsg==WM_LBUTTONDOWN || uMsg==WM_RBUTTONDOWN || uMsg==WM_MBUTTONDOWN)
          e.type = Window::Event::BUTTONDOWN;
        else e.type = Window::Event::BUTTONUP;
        switch(uMsg) {
          case WM_LBUTTONDOWN:
          case WM_LBUTTONUP:
            e.button = Window::Event::LBUTTON;
            break;
          case WM_RBUTTONDOWN:
          case WM_RBUTTONUP:
            e.button = Window::Event::RBUTTON;
            break;
          case WM_MBUTTONDOWN:
          case WM_MBUTTONUP:
            e.button = Window::Event::MBUTTON;
            break;
        }
      }
      e.x = GET_X_LPARAM(lParam);
      e.y = GET_Y_LPARAM(lParam);
      e.x -= mousePos->x();
      e.y -= mousePos->y();
      *mousePos += Vector2i(e.x,e.y);
      break;
    case WM_MOUSEWHEEL:
      e.type = Window::Event::MOUSEWHEEL;
      e.y = GET_WHEEL_DELTA_WPARAM(wParam);
      break;
    case WM_SIZE: // The size of the window has changed
      e.type = Window::Event::RESIZE;
      e.x = LOWORD(lParam);
      e.y = HIWORD(lParam);
      break;
    case WM_CHAR:
      if(wParam != '\b' && wParam != '\r') {
        e.type = Window::Event::TEXT;
        strcpy(e.text,UTF16toUTF8(wParam));
      } else return 0;
      break;
    case WM_SETCURSOR:
      SetCursor((*flags & Window::nocursor) ? nullptr : LoadCursor(nullptr,IDC_ARROW));
      break;
    case WM_INPUT:
    {
      BYTE buffer[1024];
      UINT dwSize(sizeof(buffer));
      if(GetRawInputData((HRAWINPUT)lParam,RID_INPUT,buffer,&dwSize,sizeof(RAWINPUTHEADER))>1024)
        L_ERROR("RawInput buffer too small, panic!");
      const RAWINPUT* raw((RAWINPUT*)buffer);
      Device::processReport(raw->header.hDevice,raw->data.hid.bRawData,raw->data.hid.dwSizeHid);
      return DefWindowProc(hwnd,uMsg,wParam,lParam);
      break;
    }
    default:
      return DefWindowProc(hwnd,uMsg,wParam,lParam);
      break;
  }
  if(e.type!=Window::Event::NONE){
    if(e.type==Window::Event::BUTTONDOWN)
      buttonstate[e.button] = true;
    else if(e.type==Window::Event::BUTTONUP)
      buttonstate[e.button] = false;
    events->push(e);
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
  wc.hCursor = (*flags & Window::nocursor) ? nullptr : LoadCursor(nullptr,IDC_ARROW);
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
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.iPixelType = PFD_TYPE_RGBA;
  }
  return &pfd;
}

void initGlew(HINSTANCE hInstance){
  L_ONCE;
  registerClass();

  HWND hWndFake = CreateWindow("LWC","FAKE",WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
                               0,0,CW_USEDEFAULT,CW_USEDEFAULT,NULL,
                               NULL,hInstance,NULL);

  hDC = GetDC(hWndFake);

  int pf = ChoosePixelFormat(hDC,initPFD());
  if(!pf)
    L_ERROR("ChoosePixelFormat failed during Glew initialization");

  if(!SetPixelFormat(hDC,pf,&pfd))
    L_ERROR("SetPixelFormat failed during Glew initialization");

  HGLRC hRCFake = wglCreateContext(hDC);
  wglMakeCurrent(hDC,hRCFake);

  if(glewInit() != GLEW_OK)
    L_ERROR("Couldn't initialize Glew");

  wglMakeCurrent(NULL,NULL);
  wglDeleteContext(hRCFake);
  ReleaseDC(hWndFake,hDC);
  DestroyWindow(hWndFake);
}


void Window::open(const char* title,int width,int height,int flags) {
  buttonstate = _buttonstate;
  events = &_events;
  mousePos = &_mousePos;
  ::flags = &_flags;
  if(opened()) return;
  _width = width;
  _height = height;
  _flags = flags;

  HINSTANCE hInstance(GetModuleHandle(nullptr));
  initGlew(hInstance);

  // Create window style
  DWORD wStyle = ((flags & borderless) ? (WS_POPUP) : (WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU))
    | ((flags & resizable) ? (WS_MAXIMIZEBOX) : 0)
    | ((flags & resizable) ? (WS_SIZEBOX) : 0)
    | WS_VISIBLE;

  // Find out needed window size for wanted client area size
  RECT rect = {0,0,(int)width,(int)height};
  AdjustWindowRect(&rect,wStyle,false);
  width = rect.right-rect.left;
  height = rect.bottom-rect.top;

  // Create window
  hWND = CreateWindow("LWC",title,wStyle,
                      CW_USEDEFAULT,CW_USEDEFAULT,width,height,
                      nullptr,nullptr,hInstance,nullptr);

  _mousePos = Vector2i(width/2,height/2);
  SetCursorPos(width/2,height/2);

  hDC = GetDC(hWND);
#define L_CHECK_EXTENSION(ext) if(!ext) L_ERRORF("OpenGL extension %s is unavailable yet necessary, make sure you're running on your dedicated graphics card.",#ext)
  L_CHECK_EXTENSION(WGLEW_ARB_create_context);
  L_CHECK_EXTENSION(WGLEW_ARB_pixel_format);
  L_CHECK_EXTENSION(GLEW_ARB_direct_state_access);

  const int iPixelFormatAttribList[] =
  {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0 // End of attributes list
  };
  int iContextAttribs[] =
  {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    0 // End of attributes list
  };

  int iPixelFormat,iNumFormats;
  wglChoosePixelFormatARB(hDC,iPixelFormatAttribList,NULL,1,&iPixelFormat,(UINT*)&iNumFormats);

  // PFD seems to be only redundant parameter now
  if(!SetPixelFormat(hDC,iPixelFormat,initPFD()))
    L_ERROR("SetPixelFormat failed during window initialization");

  if(!(hRC = wglCreateContextAttribsARB(hDC,0,iContextAttribs)))
    L_ERROR("wglMakeCurrent failed");
  wglMakeCurrent(hDC,hRC);
}
void Window::close() {
  L_ASSERT(opened());
  DestroyWindow(hWND);
  hWND = 0;
}
bool Window::opened() { return hWND!=0; }
bool Window::loop() {
  MSG msg;
  if(_flags&loopcursor){
    bool changed(false);
    if(_mousePos.x()<=0)
      _mousePos.x() = _width-5,changed = true;
    else if(_mousePos.x()>=_width-1)
      _mousePos.x() = 5,changed = true;
    if(_mousePos.y()<=0)
      _mousePos.y() = _height-5,changed = true;
    else if(_mousePos.y()>=_height-1)
      _mousePos.y() = 5,changed = true;
    if(changed) SetCursorPos(_mousePos.x(),_mousePos.y());
  }
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
  _width = width;
  _height = height;
  SetWindowPos(hWND,HWND_NOTOPMOST,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER);
}
void Window::mousePosition(const Vector2i& p){
  SetCursorPos(p.x(),p.y());
}