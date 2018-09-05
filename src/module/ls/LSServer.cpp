#include "LSServer.h"

#include <L/src/dev/profiling.h>
#include <L/src/stream/CFileStream.h>
#include <L/src/stream/BufferStream.h>

using namespace L;
using namespace Script;

void LSServer::update() {
  L_SCOPE_MARKER("LSServer::update");
  { // Accept new connections
    SOCKET socket;
    while(new_client(socket)) {
      _clients[socket];
    }
  }

  for(auto& pair : _clients) {
    SOCKET socket(pair.key());
    Client& client(pair.value());
    char buffer[2<<10];
    if(int byte_count = Network::recv(socket, buffer, sizeof(buffer))) {
      if(byte_count==-1) { // Connection closed
        _clients.remove(pair.key());
      } else { // We could read something
        client.compiler.read(buffer, byte_count, false);
        if(client.compiler.ready()) { // There was a line ending
          // Execute command
          Ref<CodeFunction> code_function(ref<CodeFunction>(client.compiler.function()));
          Var result(client.context.executeInside(Array<Var>{code_function}));

          // Send back result
          {
            BufferStream buffer_stream(buffer, sizeof(buffer));
            buffer_stream << result << '\n';
            Network::send(socket, buffer, buffer_stream.tell());
          }

          // Reset compiler
          client.compiler.reset();
        }
      }
    }
  }
}
