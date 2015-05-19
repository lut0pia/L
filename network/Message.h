#ifndef DEF_L_Network_Message
#define DEF_L_Network_Message

#include "../containers/Ref.h"
#include "../containers/Array.h"
#include "../macros.h"
#include "Network.h"

namespace L {
  namespace Network {
    class Message {
      private:
        Ref<Array<byte> > v;

      public:
        Message() {}
        Message(const Message&);
        Message(const Array<byte>&);
        Message(const String&);
        Message(Ref<Array<byte> >);

        inline Array<byte>& gVector() {return *v;}
        void send(SOCKET);
        void recv(SOCKET);

    };
  }
}
#endif

