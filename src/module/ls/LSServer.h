#pragma once

#include <L/src/container/Table.h>
#include <L/src/network/Server.h>
#include <L/src/script/Context.h>
#include "LSCompiler.h"

class LSServer : protected L::Server {
protected:
  struct Client {
    LSCompiler compiler;
    L::Script::Context context;
  };
  L::Table<SOCKET, Client> _clients;
public:
  inline LSServer(short port) : L::Server(port) {}
  void update();
};
