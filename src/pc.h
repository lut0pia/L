#pragma once

// CSTD
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstddef>
#include <cmath>
#include <ctime>

// STL
#include <algorithm>
#include <functional>
#include <limits>
#include <utility>

// OpenGL
#include <GL/glew.h>

// Platform
#if defined L_WINDOWS
# undef _WIN32_WINNT
# define _WIN32_WINNT 0x501
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
# include <windowsx.h>
# include <mmsystem.h>
# include <sys/timeb.h>
# undef near
# undef far
# define popen _popen
# define pclose _pclose
#elif defined L_UNIX
# include <unistd.h>
# include <curses.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <pthread.h>
#endif