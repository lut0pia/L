#include "LSServer.h"

#include <L/src/dev/profiling.h>
#include <L/src/stream/CFileStream.h>
#include <L/src/stream/BufferStream.h>

using namespace L;

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
        if(client.compiler.read(buffer, byte_count, false)) { // The code is compilable
          // Compile code
          ScriptFunction script_function(client.compiler.compile());

          // Execute command
          const Var result(client.context.execute(ref<ScriptFunction>(script_function)));

          // Send back result
          {
            BufferStream buffer_stream(buffer, sizeof(buffer));
            buffer_stream << result << '\n';
            Network::send(socket, buffer, buffer_stream.tell());
          }
        }
      }
    }
  }
}
