#include "ScriptServer.h"

#include "../streams/CFileStream.h"
#include "../streams/BufferStream.h"

using namespace L;
using namespace Network;
using namespace Script;

void ScriptServer::update() {
  static const size_t client_buffer_size(2<<12);

  {
    SOCKET socket;
    while(new_client(socket)) {
      Client& client(_clients[socket]);
      client._buffer = (char*)Memory::alloc(client_buffer_size);
      client._pos = 0;
    }
  }

  for(auto& pair : _clients) {
    SOCKET socket(pair.key());
    Client& client(pair.value());
    char* read_start(client._buffer+client._pos);
    size_t read_size(client_buffer_size-client._pos);
    L_ASSERT(read_size>0);
    if(int byte_count = recv(socket, read_start, read_size)) {
      if(byte_count==-1) { // Connection closed
        Memory::free(client._buffer, client_buffer_size);
        _clients.remove(pair.key());
      } else { // We could read something
        client._pos += byte_count;
        if(char* end_line = (char*)memchr(read_start, '\n', byte_count)) { // There was a line ending
          // Execute command
          Ref<CodeFunction> code_function(ref<CodeFunction>(Context::read(BufferStream(client._buffer, end_line))));
          Var result(_script_context.executeInside(Array<Var>{code_function}));

          // Send back result
          {
            char buffer[2<<10];
            BufferStream buffer_stream(buffer, sizeof(buffer));
            buffer_stream << result << '\n';
            out.write(buffer, buffer_stream.pos());
            send(socket, buffer, buffer_stream.pos());
          }

          // Reset buffer for future reads
          memmove(client._buffer, end_line, byte_count-(end_line-read_start));
          client._pos = 0;
        }
      }
    }
  }
}
