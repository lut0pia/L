#include "Message.h"

#include "../bytes.h"
#include "../Exception.h"

using namespace L;
using namespace Network;

Message::Message(const Message& message){
    v = message.v;
}
Message::Message(const Vector<byte>& message){
    v = new Vector<byte>(message);
}
Message::Message(const String& message){
    v = new Vector<byte>(message.begin(),message.end());
}
Message::Message(Ref<Vector<byte> > message){
    v = message;
}

void Message::send(SOCKET sock){
    if(v!=NULL){
        Vector<byte> sizeVec(uitb(v->size(),2));
        uint i, sent;
        if(::send(sock,(char*)&sizeVec[0],2,0) == -1){
            #if defined L_WINDOWS
                throw Exception("Send error " + WSAGetLastError());
            #endif
        }
        else{
            i=0;
            while(i<v->size()){
                if((sent = ::send(sock,(char*)&(*v)[i],v->size()-i,0)) == (uint)-1){
                    #if defined L_WINDOWS
                        throw Exception("Send error " + WSAGetLastError());
                    #endif
                }
                i += sent;
            }
        }
    }
}
void Message::recv(SOCKET sock){
    size_t size,recvd,i;
    Vector<byte> sizeVec(2);
    if(::recv(sock,(char*)&sizeVec[0],2,0) != 2)
        throw Exception("Recv error 1");
    else{
        i=recvd=0;
        v = new Vector<byte>(size = btui(sizeVec));
        while(i<size && (recvd = ::recv(sock,(char*)&(*v)[i],size-i,0)) > 0 && recvd<=size-i) // 0 means connection stopped
            i+=recvd;
        if(recvd<=0){
            if(recvd<0)
                throw Exception("Recv error 2");
        }
    }
}


