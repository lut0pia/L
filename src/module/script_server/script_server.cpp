#include <L/src/engine/Engine.h>
#include <L/src/engine/Resource.inl>
#include <L/src/network/Server.h>
#include <L/src/script/ScriptContext.h>

using namespace L;

static void script_server_update() {
  struct Client {
    ScriptContext context;
    Symbol language;
    String code_buffer, line_buffer;
  };
  static L::Table<SOCKET, Client> clients;
  static Server server = 1993;

  L_SCOPE_MARKER("script_server_update");

  { // Accept new connections
    SOCKET socket;
    while(server.new_client(socket)) {
      clients[socket];
    }
  }

  for(auto& pair : clients) {
    SOCKET socket = pair.key();
    Client& client = pair.value();

    { // Try to read from socket
      char buffer[2 << 10];
      const int byte_count = Network::recv(socket, buffer, sizeof(buffer));
      if(byte_count == -1) { // Connection closed
        clients.remove(pair.key());
        continue;
      } else if(byte_count == 0) { // Couldn't read anything
        continue;
      }
      client.line_buffer += String(buffer, byte_count);
    }

    // Support DEL chars for telnet clients
    while(const char* del_char = strchr(client.line_buffer.begin(), 127)) {
      const intptr_t index = max<intptr_t>(0, del_char - client.line_buffer.begin() - 1);
      const size_t size = min<size_t>(2, client.line_buffer.size() - index);
      client.line_buffer.erase(index, size);
      Network::send(socket, "\b \b", 3);
    }

    // Parse lines
    while(const char* line_end = strchr(client.line_buffer.begin(), '\n')) {
      const uintptr_t line_end_index = line_end - client.line_buffer.begin();
      String line = String(client.line_buffer.begin(), line_end_index);
      const bool using_crlf = line_end - 1 >= client.line_buffer.begin() && *(line_end - 1) == '\r';
      client.line_buffer.erase(0, line_end_index + 1);
      line.trim('\r');

      if(line.size() == 0) { // Execute
        if(!client.language) {
          warning("script_server: No language set");
          continue;
        }

        if(client.code_buffer.size() == 0) {
          continue;
        }

        ResourceSlot res_slot("", "");
        res_slot.source_buffer = Buffer(client.code_buffer.begin(), client.code_buffer.size());
        res_slot.ext = client.language;

        Ref<ScriptFunction> script_function = ref<ScriptFunction>();
        if(ResourceLoading<ScriptFunction>::load_internal(res_slot, *script_function)) {
          const Var result = client.context.execute(script_function);

          // Send back result
          {
            StringStream string_stream;
            const char* new_line = (using_crlf ? "\r\n" : "\n");
            string_stream << result << new_line << new_line;
            Network::send(socket, string_stream.string().begin(), string_stream.string().size());
          }
        }

        client.code_buffer.clear();
      } else if(line[0] == '#') { // Directive
        if(line[1] == '!') {
          client.language = line.begin() + 2;
          client.code_buffer.clear();
        }
      } else { // Code
        client.code_buffer += line + "\n";
      }
    }
  }
}

void script_server_module_init() {
  Engine::add_update(script_server_update);
}
