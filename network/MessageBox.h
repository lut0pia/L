#ifndef DEF_L_Network_MessageBox
#define DEF_L_Network_MessageBox

#include "Message.h"
#include "../parallelism.h"

namespace L{
    namespace Network{
        class MessageBox{
            protected:
                SOCKET sd;
                bool connected;
                List<Vector<byte> > recvQueue;
                List<Message> sendQueue;

                Thread recvMessagesThread,
                       sendMessagesThread;
                Mutex recvQueueMutex,
                      sendQueueMutex;
                Semaphore sendQueueSemaphore;
            public:
                MessageBox(SOCKET);
                L_NoCopy(MessageBox)
                ~MessageBox();
                void disconnect();

                void queueMessage(Message); // Puts message in queue and sends it after the rest
                void sendMessage(Message); // Sends message as soon as possible
                void clearSending(); // Clears the sending queue

                bool newMessage(); // Returns true if there are messages in the receiving queue
                const Vector<byte>& gMessage(); // Returns the content of the message
                void popMessage(); // Deletes the message

                inline SOCKET gSD(){return sd;}
                inline bool isConnected(){return connected;}

            protected:
                static Dynamic::Var recvMessages(Thread*);
                static Dynamic::Var sendMessages(Thread*);
        };
    }
}

#endif
