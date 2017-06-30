#include "ScriptServer.h"

#include "../streams/CFileStream.h"
#include "../streams/BufferStream.h"

using namespace L;
using namespace Network;
using namespace Script;

void ScriptServer::update() {
  static const size_t client_buffer_size(4096);

  {
    SOCKET socket;
    while(new_client(socket)) {
      auto& client(_clients[socket]);
      client._buffer = (char*)Memory::alloc(client_buffer_size);
      client._pos = 0;
    }
  }

  for(auto& pair : _clients) {
    auto& client(pair.value());
    char* read_start(client._buffer+client._pos);
    size_t read_size(client_buffer_size-client._pos);
    L_ASSERT(read_size>0);
    if(int result = recv(pair.key(), read_start, read_size)) {
      if(result==-1) { // Connection closed
        Memory::free(client._buffer, client_buffer_size);
        _clients.remove(pair.key());
      } else { // We could read something
        client._pos += result;
        if(char* end_line = (char*)memchr(read_start, '\n', result)) { // There was a line ending
          // Print command to terminal
          out.write(client._buffer, end_line-client._buffer);
          out << '\n';

          // Execute command
          BufferStream buffer_stream(client._buffer, end_line);
          Ref<CodeFunction> code_function(ref<CodeFunction>(Context::read(buffer_stream)));
          _script_context.executeInside(Array<Var>{code_function});

          // Reset buffer for future reads
          memmove(client._buffer, end_line, result-(end_line-read_start));
          client._pos = 0;
        }
      }
    }
  }
}
