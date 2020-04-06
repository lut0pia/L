#include "Network.h"

using namespace L;

void Network::init() {
  static WSADATA WSAData;
  WSAStartup(MAKEWORD(2, 0), &WSAData);
}
int Network::error_code() { return WSAGetLastError(); }
bool Network::would_block() { return error_code() == WSAEWOULDBLOCK; }
void Network::make_non_blocking(SOCKET sd) {
  static u_long mode(1);
  int error_code;
  if((error_code = ioctlsocket(sd, FIONBIO, &mode)) != NO_ERROR)
    error("ioctlsocket error: %d", error_code);
}
