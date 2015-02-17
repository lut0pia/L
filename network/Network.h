#ifndef DEF_L_Network
#define DEF_L_Network

    /*  Windows libraries :
    **      - ws2_32
    */

#include <iostream>
#include "../stl.h"

#include "../macros.h"
#if defined L_WINDOWS
    #undef _WIN32_WINNT
    #define _WIN32_WINNT 0x501
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
    #include <stdio.h>
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

namespace L{
    namespace Network{
        void init();
        SOCKET connectTo(const String& port, const String& ip);
        Set<String> DNSLookup(const String& host);
        String HTTPRequest(const String& url);
        void HTTPDownload(const String& url, const String& name);
    }
}

#endif





