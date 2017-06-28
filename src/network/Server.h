#pragma once

#include "Network.h"

namespace L {
  class Server {
  protected:
    SOCKET _sd;
    fd_set _listen_fds;
  public:
    Server(short port);
    ~Server();
    bool new_client(SOCKET&);
  };
}
