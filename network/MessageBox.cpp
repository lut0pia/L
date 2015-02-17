#include "MessageBox.h"

using namespace L;
using namespace Network;

#include "Message.h"

MessageBox::MessageBox(SOCKET sd) : sendQueueSemaphore(0){
    this->sd = sd;
    connected = true;
    recvMessagesThread.start(recvMessages,this);
    sendMessagesThread.start(sendMessages,this);
}
MessageBox::~MessageBox(){
    recvMessagesThread.terminate();
    sendMessagesThread.terminate();
}
void MessageBox::disconnect(){
    if(connected){
        closesocket(sd);
        connected = false;
    }
}

void MessageBox::queueMessage(Message message){
    sendQueueMutex.lock();
    sendQueue.push_back(message);
    sendQueueMutex.unlock();
    sendQueueSemaphore.post();
}
void MessageBox::sendMessage(Message message){
    sendQueueMutex.lock();
    sendQueue.push_front(message);
    sendQueueMutex.unlock();
    sendQueueSemaphore.post();
}
void MessageBox::clearSending(){
    sendQueueMutex.lock();
    sendQueue.clear();
    sendQueueMutex.unlock();
}

bool MessageBox::newMessage(){
    return !recvQueue.empty();
}
const Vector<byte>& MessageBox::gMessage(){
    return recvQueue.front();
}
void MessageBox::popMessage(){
    recvQueueMutex.lock();
    if(newMessage())
        recvQueue.pop_front();
    recvQueueMutex.unlock();
}

Dynamic::Var MessageBox::recvMessages(Thread* thread){
    MessageBox& mb = *thread->gArg().as<MessageBox*>();
    SOCKET sd = mb.sd;
    Message message;
    while(mb.connected){
        try{message.recv(sd);}
        catch(...){break;}
        mb.recvQueueMutex.lock();
        mb.recvQueue.push_back(message.gVector());
        mb.recvQueueMutex.unlock();
    }
    mb.disconnect();
    return 0;
}
Dynamic::Var MessageBox::sendMessages(Thread* thread){
    MessageBox& mb = *thread->gArg().as<MessageBox*>();
    SOCKET sd = mb.sd;
    List<Message>& mbq = mb.sendQueue;
    while(mb.connected){
        mb.sendQueueSemaphore.wait();
        mb.sendQueueMutex.lock();
        while(!mbq.empty()){
            Message message(mbq.front());
            mbq.pop_front();
            mb.sendQueueMutex.unlock();
            try{message.send(sd);}
            catch(...){
                mb.disconnect();
                break;
            }
            mb.sendQueueMutex.lock();
        }
        mb.sendQueueMutex.unlock();
    }
    return 0;
}
