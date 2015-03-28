#ifndef DEF_L_Network_Message
#define DEF_L_Network_Message

#include "../containers/Ref.h"
#include "../macros.h"
#include "Network.h"

namespace L {
  namespace Network {
    class Message {
      private:
        Ref<Vector<byte> > v;

      public:
        Message() {}
        Message(const Message&);
        Message(const Vector<byte>&);
        Message(const String&);
        Message(Ref<Vector<byte> >);

        inline Vector<byte>& gVector() {return *v;}
        void send(SOCKET);
        void recv(SOCKET);

    };
  }
}
#endif

