#pragma once

/*  Windows libraries :
**      - ws2_32
*/

#include "../containers/Set.h"
#include "../text/String.h"

#include "../macros.h"
#if defined L_WINDOWS
typedef int socklen_t;
#include <stdio.h>
#include <WinSock2.h>
#elif defined L_UNIX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define closesocket(sock) close(sock)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#endif
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif

namespace L {
  namespace Network {
    // System-specific
    void init();
    int error();
    bool would_block();
    void make_non_blocking(SOCKET);
  
    // Generic
    SOCKET connect_to(const char* ip, short port);
    int recv(SOCKET, char* buffer, size_t size);
    bool send(SOCKET, const char* buffer, size_t size);
    Set<String> dns_lookup(const char* host);

    // HTTP
    String HTTPRequest(const String& url);
    void HTTPDownload(const char* url, const char* name);
  }
}
