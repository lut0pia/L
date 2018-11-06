#pragma once

// CSTD
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <cfloat>
#include <ctime>

// STL
#include <initializer_list>
#include <limits>

// Platform
#ifdef L_UNIX
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
# include <alsa/asoundlib.h>
#endif
