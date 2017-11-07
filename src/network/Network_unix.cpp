#include "Network.h"

#include <errno.h>

using namespace L;

void Network::init() {}
int Network::error_code() { return errno; }
bool Network::would_block() {
  int err(error_code());
  return err == EWOULDBLOCK || err == EAGAIN;
}
void Network::make_non_blocking(SOCKET sd) {
  fcntl(sd, F_SETFL, O_NONBLOCK);
}
