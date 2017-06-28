#include "Server.h" 

using namespace L;
using namespace Network;

#define far
#define near

Server::Server(short port) {
  if((_sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    L_ERRORF("Server socket error %d", error());

  { // Set reuseaddr
    char optval = '1';
    setsockopt(_sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  }

  make_non_blocking(_sd);

  {
    SOCKADDR_IN sin;
    memset(&sin, 0, sizeof sin);
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if(bind(_sd, (SOCKADDR*)&sin, sizeof(sin)))
      L_ERRORF("Server bind error %d", error());
  }

  if(listen(_sd, 16) == -1)
    L_ERRORF("Server listen error %d", error());

  FD_ZERO(&_listen_fds);
  FD_SET(_sd, &_listen_fds);
}
Server::~Server() {
  closesocket(_sd);
}
bool Server::new_client(SOCKET& sd) {
  static timeval tv{};
  fd_set fds(_listen_fds);

  if(select(_sd+1, &fds, nullptr, nullptr, &tv)==-1)
    L_ERRORF("Server select error %d", error());

  if(FD_ISSET(_sd, &fds)) {
    struct sockaddr_storage their_addr;
    socklen_t sin_size(sizeof(their_addr));

    if((sd = accept(_sd, (struct sockaddr *)&their_addr, &sin_size)) == SOCKET(-1))
      L_ERRORF("Server accept error %d", error());

    make_non_blocking(sd);

    return true;
  } else return false;
}
