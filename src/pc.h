#pragma once

// CSTD
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <ctime>

// STL
#include <algorithm>
#include <functional>
#include <limits>
#include <utility>

// OpenGL
#include <GL/glew.h>
#include <GL/glu.h>

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
# include <signal.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <pthread.h>
#endif
