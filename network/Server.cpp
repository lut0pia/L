#include "Server.h"

#include "../Exception.h"

using namespace L;
using namespace Network;
/*
#if defined L_UNIX
    #include <string.h>
#endif

Server::Server(const char* port){
    char optval = '1';
    sd = 0;

    SOCKADDR_IN sin;
    if((sd = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET){ // INVALID_SOCKET
        #if defined L_WINDOWS
            throw Exception("Server socket error " + WSAGetLastError());
        #endif
        sd = 0;
        return;
    }
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval));
    memset(&sin, 0, sizeof sin);
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(port));

    if(::bind(sd,(SOCKADDR*)&sin,sizeof(sin))){
        printf("%s",strerror(errno));
        throw Exception("Server bind error");
        closesocket(sd);
        sd = 0;
        return;
    }

    if(listen(sd, 5) == -1){
        throw Exception("Server listen error");
        closesocket(sd);
        sd = 0;
        return;
    }
    waitForClientsThread.start(waitForClients,this);
}

bool Server::newClient(){
    return !connectionQueue.empty();
}
SOCKET Server::gNewClient(){
    connectionQueueMutex.lock();
    SOCKET wtr = connectionQueue.front();
    connectionQueue.pop_front();
    connectionQueueMutex.unlock();
    return wtr;
}

bool Server::connected(){
    return (sd > 0);
}

Dynamic::Var Server::waitForClients(Thread* thread){
    Server& server = *thread->gArg().as<Server*>();
    SOCKET sockfd = server.sd, newfd;
    struct sockaddr_storage their_addr; // client's address information
    socklen_t sin_size;
    sin_size = sizeof(their_addr);
    while(true){ // accept loop
        if((newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) != (uint)-1){ // New client
            server.connectionQueueMutex.lock();
            server.connectionQueue.push_back(newfd); // Add connection to queue
            server.connectionQueueMutex.unlock();
        }
        else throw Exception("Server accept error");
    }
    return 0;
}
*/
