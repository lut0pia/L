#pragma once

#include "../container/Table.h"
#include "../network/Server.h"
#include "../script/Compiler.h"
#include "../script/Context.h"

namespace L {
  class ScriptServer : protected Server {
  protected:
    struct Client {
      Script::Compiler compiler;
      Script::Context context;
    };
    Table<SOCKET, Client> _clients;
  public:
    inline ScriptServer(short port) : Server(port) {}
    void update();
  };
}
