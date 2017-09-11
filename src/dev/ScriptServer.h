#pragma once

#include "../container/Table.h"
#include "../network/Server.h"
#include "../script//Context.h"

namespace L {
  class ScriptServer : protected Server {
  protected:
    struct Client{
      char* _buffer;
      uintptr_t _pos;
    };
    Table<SOCKET, Client> _clients;
    Script::Context _script_context;
  public:
    inline ScriptServer(short port) : Server(port) {}
    void update();
  };
}
