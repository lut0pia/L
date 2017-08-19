#include "Network.h"

using namespace L;
using namespace Network;

#include "../stream/NetStream.h"
#include "../stream/CFileStream.h"

SOCKET Network::connect_to(const char* ip, short port) {
  SOCKET sd(0);
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    L_ERRORF("Couldn't create socket to %s:%hd - %d", ip, port, error());

  {
    SOCKADDR_IN sin;
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if(connect(sd, (SOCKADDR*)&sin, sizeof(sin)) < 0)
      L_ERRORF("Couldn't connect to %s:%hd - %d", ip, port, error());
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
  int result(0);
  while(size && (result = ::send(sd, buffer, int(size), 0))) {
    buffer += result;
    size -= result;
  }
  return result != 0;
}
Set<String> Network::dns_lookup(const char* host) {
  struct addrinfo hints, *res, *p;
  Set<String> wtr;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;
  if(!getaddrinfo(host, nullptr, &hints, &res)) {
    for(p = res; p != nullptr; p = p->ai_next) {
      // get the pointer to the address itself,
      // different fields in IPv4 and IPv6:
      if(p->ai_family == AF_INET) { // IPv4
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
  }
  return wtr;
}

String Network::HTTPRequest(const String& url) {
  size_t tmp;
  char buffer[1024];
  String wtr;
  SOCKET sd;
  // Find out what's the host and what's the request
  int slash(url.findFirst('/'));
  String host((slash>=0) ? url.substr(0, slash) : url), request((slash>=0) ? url.substr(slash) : "/");
  // Connect to the server
  Set<String> ips(dns_lookup(host));
  if(!ips.empty()) {
    NetStream test(sd = connect_to(ips[0], 80));
    test << "GET " << request << " HTTP/1.1\r\nHost: " << host << "\r\nConnection: close\r\n\r\n";
    while((tmp = ::recv(sd, buffer, 1024, 0)))
      wtr += String(buffer, tmp);
    return wtr;
  } else L_ERRORF("Could not find ip for %s", (const char*)host);
}
void Network::HTTPDownload(const char* url, const char* name) {
  String answer(HTTPRequest(url));
  CFileStream file(name, "wb");
  file << String(answer, answer.findFirst("\r\n\r\n")+4);
}
