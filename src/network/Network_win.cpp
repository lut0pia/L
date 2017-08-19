#include "Network.h"

using namespace L;

void Network::init() {
  L_ONCE;
  static WSADATA WSAData;
  WSAStartup(MAKEWORD(2, 0), &WSAData);
}
int Network::error() { return WSAGetLastError(); }
bool Network::would_block() { return error() == WSAEWOULDBLOCK; }
void Network::make_non_blocking(SOCKET sd) {
  static u_long mode(1);
  int error;
  if((error = ioctlsocket(sd, FIONBIO, &mode)) != NO_ERROR)
    L_ERRORF("ioctlsocket error: %d", error);
}
