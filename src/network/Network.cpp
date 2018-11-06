#include "Network.h"

#include <WS2tcpip.h>

using namespace L;
using namespace Network;

#include "../stream/NetStream.h"
#include "../stream/CFileStream.h"

SOCKET Network::connect_to(const char* ip, short port) {
  return connect_to(inet_addr(ip), port);
}
SOCKET Network::connect_to(uint32_t addr, short port) {
  SOCKET sd(0);
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    error("Couldn't create socket to %s:%hd - %d", inet_ntoa((in_addr&)addr), port, error_code());

  {
    SOCKADDR_IN sin;
    sin.sin_addr.s_addr = addr;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if(connect(sd, (SOCKADDR*)&sin, sizeof(sin)) < 0)
      error("Couldn't connect to %s:%hd - %d", inet_ntoa((in_addr&)addr), port, error_code());
  }
  make_non_blocking(sd);
  return sd;
}
int Network::recv(SOCKET sd, char* buffer, size_t size) {
  const int result(::recv(sd, buffer, int(size), 0));
  if(result<0 && would_block()) // Nothing to read
    return 0;
  else if(result==0) // Connection closed
    return -1;
  else return result;
}
bool Network::send(SOCKET sd, const char* buffer, size_t size) {
  while(size) {
    int result(::send(sd, buffer, int(size), 0));
    if(result == 0) break; // Failure
    buffer += result;
    size -= result;
  }
  return size == 0;
}
uint32_t Network::dns_lookup(const char* host) {
  struct addrinfo hints, *res, *p;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;
  if(!getaddrinfo(host, nullptr, &hints, &res)) {
    for(p = res; p != nullptr; p = p->ai_next) {
      // get the pointer to the address itself,
      // different fields in IPv4 and IPv6:
      if(p->ai_family == AF_INET) { // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
        return ipv4->sin_addr.s_addr;
      }
    }
    freeaddrinfo(res); // free the linked list
  }
  return 0;
}

String Network::http_request(const String& url) {
  // Find out what's the host and what's the request
  int slash(url.find_first('/'));
  const String host((slash>=0) ? url.substr(0, slash) : url), request((slash>=0) ? url.substr(slash) : "/");
  // Connect to the server
  const uint32_t addr(dns_lookup(host));
  String wtr;
  if(addr) {
    SOCKET sd;
    NetStream test(sd = connect_to(addr, 80));
    test << "GET " << request << " HTTP/1.1\r\nHost: " << host << "\r\nConnection: close\r\n\r\n";
    char buffer[1024];
    while(int tmp = ::recv(sd, buffer, sizeof(buffer), 0))
      if(tmp>0) {
        wtr.size(wtr.size()+tmp);
        char* write_start(wtr.end()-tmp);
        memcpy(write_start, buffer, tmp);
      }
  } else error("Could not find ip for %s", (const char*)host);
  return wtr;
}
void Network::http_download(const char* url, const char* name) {
  String answer(http_request(url));
  CFileStream file(name, "wb");
  file << String(answer, answer.find_first("\r\n\r\n")+4);
}
