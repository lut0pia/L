#include "Window.h"

#include <cstring>
#include "../stl.h"
#include "../bytes/encoding.h"

#if defined L_WINDOWS
    #include <windows.h>
    #include <windowsx.h>
#elif defined L_UNIX
    #include<X11/X.h>
    #include<X11/Xlib.h>
    #include<GL/gl.h>
    #include<GL/glx.h>
    #include<GL/glu.h>
#endif

using namespace L;
using L::Window;

Vector<bool> keystate(Window::Event::LAST, false);
List<Window::Event> events;
Point2i mousePos;

Window::Event::Event(){
    memset(this,0,sizeof(*this));
}

#if defined L_WINDOWS
    HWND hWND;
    HDC hDC;
    HGLRC hRC;
    LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
        Window::Event e;
        switch(uMsg){
            case WM_CREATE:
                return 0;
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                hWND = 0;
                return 0;
                break;
            case WM_KEYDOWN: case WM_KEYUP: // Key pressed
                e.type = (uMsg==WM_KEYDOWN) ? Window::Event::KEYDOWN : Window::Event::KEYUP;
                switch(wParam){
                    #define MAP(a,b) case a: e.key = Window::Event::b; break;
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
                            e.key = (Window::Event::VKey)wParam;
                        else return 0;
                }
                keystate[e.key] = (uMsg==WM_KEYDOWN);
                break;
            case WM_MOUSEMOVE: case WM_LBUTTONDOWN: case WM_RBUTTONDOWN:
                switch(uMsg){
                    case WM_LBUTTONDOWN : e.type = Window::Event::LBUTTONDOWN; break;
                    case WM_RBUTTONDOWN : e.type = Window::Event::RBUTTONDOWN; break;
                    default: e.type = Window::Event::MOUSEMOVE; break;
                }
                ({
                    Point2i pos;
                    e.x = pos.x() = GET_X_LPARAM(lParam);
                    e.y = pos.y() = GET_Y_LPARAM(lParam);
                    mousePos = pos;
                });
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
                if(wParam != '\b' && wParam != '\r'){
                    e.type = Window::Event::TEXT;
                    strcpy(e.text,ANSItoUTF8(String(1,wParam)).c_str());
                }
                else return 0;
                break;
            default: return DefWindowProc(hwnd, uMsg, wParam, lParam); break;
        }
        events.push_back(e);
        return 0;
    }
#elif defined L_UNIX
    bool                    winOpened(false);
    Display                 *dpy;
    ::Window                root;
    GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    XVisualInfo             *vi;
    Colormap                cmap;
    XSetWindowAttributes    swa;
    ::Window                win;
    GLXContext              glc;
    XWindowAttributes       gwa;
    void eventTranslate(const XEvent& xev){
        Window::Event e;
        switch(xev.type){
            case MotionNotify:
                e.type = Window::Event::MOUSEMOVE;
                e.x = xev.xmotion.x;
                e.y = xev.xmotion.y;
                break;
            case KeyPress:

                break;
            case ResizeRequest:
                e.type = Window::Event::RESIZE;
                e.x = xev.xresizerequest.width;
                e.y = xev.xresizerequest.height;
                XResizeWindow(dpy,win,e.x,e.y);
                break;
            case ClientMessage: // It's the close operation
                Window::close();
                break;
            default: return; break;
        }
        events.push_back(e);
    }
#endif

void Window::open(const String& title, size_t width, size_t height, size_t flags){
    if(opened()) return;
    #if defined L_WINDOWS
        WNDCLASS wc;

        PIXELFORMATDESCRIPTOR pfd;
        int format;

        // Register window class
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = MainWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL,IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "LWC";
        RegisterClass(&wc);

        DWORD wStyle = WS_CAPTION
                     | WS_MINIMIZEBOX
                     | ((flags & resizable)?(WS_MAXIMIZEBOX):0)
                     | ((flags & resizable)?(WS_SIZEBOX):0)
                     | WS_SYSMENU | WS_VISIBLE;

        // Find out needed window size for wanted client area size
        RECT rect = {0,0,(int)width,(int)height};
        AdjustWindowRect(&rect,wStyle,false);
        width = rect.right-rect.left;
        height = rect.bottom-rect.top;

        // Create window
        hWND = CreateWindow("LWC",title.c_str(),wStyle,   // Properties
                CW_USEDEFAULT,CW_USEDEFAULT,width,height,
                NULL,NULL,GetModuleHandle(NULL),NULL);

        // Set OpenGL as renderer
        hDC = GetDC(hWND); // get the device context (DC)
        ZeroMemory(&pfd,sizeof(pfd)); // set the pixel format for the DC
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cDepthBits = 16;
        pfd.iLayerType = PFD_MAIN_PLANE;
        format = ChoosePixelFormat(hDC,&pfd);
        SetPixelFormat(hDC,format,&pfd);
        hRC = wglCreateContext(hDC); // create and enable the render context (RC)
        wglMakeCurrent(hDC,hRC);
    #elif defined L_UNIX
        if((dpy = XOpenDisplay(NULL)) == NULL)
            throw Exception("Cannot open X server display.");
        root = DefaultRootWindow(dpy);
        if((vi = glXChooseVisual(dpy, 0, att)) == NULL)
            throw Exception("No appropriate visual found for X server.");
        cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
        swa.colormap = cmap;
        swa.event_mask = ExposureMask | KeyPressMask | PointerMotionMask;
        win = XCreateWindow(dpy, root, 0, 0, width, height, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

        Atom delWindow = XInternAtom(dpy,"WM_DELETE_WINDOW",0); // This is for the window close operation
        XSetWMProtocols(dpy, win, &delWindow, 1);

        XMapWindow(dpy, win);
        XStoreName(dpy, win, title.c_str());
        glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
        glXMakeCurrent(dpy, win, glc);

        winOpened = true;
    #endif
}
void Window::close(){
    if(!opened()) return;
    #if defined L_WINDOWS
        DestroyWindow(hWND);
        hWND = 0;
    #elif defined L_UNIX
        glXMakeCurrent(dpy, None, NULL);
        glXDestroyContext(dpy, glc);
        XDestroyWindow(dpy, win);
        XCloseDisplay(dpy);
        winOpened = false;
    #endif
}
bool Window::opened(){
    #if defined L_WINDOWS
        return (hWND);
    #elif defined L_UNIX
        return (winOpened);
    #endif
    return false;
}
bool Window::newEvent(Event& e){
    #if defined L_WINDOWS
        MSG msg;
        while(opened() && PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    #elif defined L_UNIX
        XEvent xev;
        while(opened() && XPending(dpy)){
            XNextEvent(dpy,&xev);
            eventTranslate(xev);
        }
    #endif
    if(events.empty())
        return false;
    else{
        e = events.front();
        events.pop_front();
        return true;
    }
}
bool Window::isPressed(Event::VKey key){
    return keystate[key];
}
void Window::swapBuffers(){
    if(!opened()) return;
    #if defined L_WINDOWS
        SwapBuffers(hDC);
    #elif defined L_UNIX
        glXSwapBuffers(dpy, win);
    #endif
}

void Window::sTitle(const String& str){
    if(!opened()) return;
    #if defined L_WINDOWS
        SetWindowText(hWND,str.c_str());
    #elif defined L_UNIX
    #endif
}
void Window::resize(size_t width, size_t height){
    if(!opened()) return;
    #if defined L_WINDOWS
        SetWindowPos(hWND,HWND_NOTOPMOST,0,0,width,height,SWP_NOMOVE|SWP_NOZORDER);
    #elif defined L_UNIX
    #endif
}
Point2i Window::gMousePos(){
    return mousePos;
}

