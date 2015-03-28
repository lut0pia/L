#ifndef DEF_L_systems
#define DEF_L_systems

#if defined(_WIN32) || defined(_WIN64)
#define L_WINDOWS
#define NOCRYPT
#define NOMINMAX
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#undef near
#undef far
#define popen _popen
#define pclose _pclose
#elif defined __unix__
#define L_UNIX
#else
#error Unknown OS
#endif

#endif
