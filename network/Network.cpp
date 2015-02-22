#include "Network.h"

using namespace L;
using namespace Network;

#include <cstring>
#include "../Exception.h"
#include "../system/File.h"
#include "Stream.h"

#if defined L_WINDOWS
    WSADATA WSAData;
#endif

void Network::init(){
    #if defined L_WINDOWS
        WSAStartup(MAKEWORD(2,0), &WSAData);
    #endif
}
SOCKET Network::connectTo(const String& port, const String& ip){
    SOCKET sd(0);

    SOCKADDR_IN sin;
    sin.sin_addr.s_addr = inet_addr(ip.c_str());
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(port.c_str()));

    if((sd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        closesocket(sd);
        throw Exception("Client socket error");
    }
    else if(connect(sd,(SOCKADDR*)&sin,sizeof(sin)) < 0){
        closesocket(sd);
        throw Exception("Client connect error");
    }

    /*
    struct addrinfo hints, *res, *i;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(ip, port, &hints, &res))
        throw Exception("Client lookup error";

    for(i = res;i != NULL;i = i->ai_next){
        if((sd = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) == INVALID_SOCKET){
            throw Exception("Client socket error";
            continue;
        }
        else if(connect(sd, i->ai_addr, i->ai_addrlen) == SOCKET_ERROR){
            throw Exception("Client connect error " << WSAGetLastError();
            closesocket(sd);
            continue;
        }
        throw Exception("Client connection success";
        break;
    }
    freeaddrinfo(res);
    */
    return sd;
}
Set<String> Network::DNSLookup(const String& host){
    struct addrinfo hints, *res, *p;
    int status;
    Set<String> wtr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if((status = getaddrinfo(host.c_str(), NULL, &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return wtr;
    }

    for(p = res;p != NULL; p = p->ai_next){
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET){ // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;

            wtr.insert(inet_ntoa(ipv4->sin_addr));
        }
        /*else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
        */
    }
    freeaddrinfo(res); // free the linked list
    return wtr;
}
String Network::HTTPRequest(const String& url){
    size_t tmp;
    char buffer[1024];
    String wtr;
    SOCKET sd;

    // Find out what's the host and what's the request
    String host(url.substr(0,url.find_first_of('/'))), request;
    if((tmp = url.find('/')) == String::npos)
        request = "/";
    else
        request = url.substr(tmp);

    // Connect to the server
    Stream test(sd = connectTo("80",DNSLookup(host).random()));

    test << "GET " << request << " HTTP/1.1\r\nHost: " << host << "\r\nConnection: close\r\n\r\n";

    while((tmp = recv(sd,buffer,1024,0)))
        wtr += String(buffer,tmp);
    return wtr;
}
void Network::HTTPDownload(const String& url, const String& name){
    String answer(HTTPRequest(url));
    File file(name);
    file.open("w");
    file.write(String(answer,answer.find("\r\n\r\n")+4));
}
