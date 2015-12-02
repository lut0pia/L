#ifndef DEF_L_systems
#define DEF_L_systems

#if defined(_WIN32) || defined(_WIN64)
# define L_WINDOWS
# define NOCRYPT
# ifndef NOMINMAX
#   define NOMINMAX
# endif
# undef _WIN32_WINNT
# define _WIN32_WINNT 0x501
# define WIN32_LEAN_AND_MEAN
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
# include <windowsx.h>
# include <mmsystem.h>
# define popen _popen
# define pclose _pclose
#elif defined __unix__
# define L_UNIX
# include <unistd.h>
# include <curses.h>
#else
# error Unknown OS
#endif

#endif
