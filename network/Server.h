#ifndef DEF_L_Network_Server
#define DEF_L_Network_Server

    /*  Windows libraries :
    **      - ws2_32
    */

#include "Network.h"
#include "../parallelism.h"

namespace L{
    namespace Network{
        class Server{
            private:
                SOCKET sd; // Local sock
                List<SOCKET> connectionQueue; // The connection queue

                Thread waitForClientsThread;
                Mutex connectionQueueMutex;

            public:
                Server(const char* port);
                L_NoCopy(Server)

                bool newClient(); // Returns true if there's a new client
                SOCKET gNewClient(); // Returns new client's socket and pops it from the queue

                bool connected();

            private:
                static Dynamic::Var waitForClients(Thread* thread);
        };
    }
}

#endif




