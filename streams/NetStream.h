#ifndef DEF_L_NetStream
#define DEF_L_NetStream

#include "FileStream.h"

namespace L {
  class NetStream {
    protected:
      int _sd;
      FileStream _readBuffer, _writeBuffer;
    public:
      NetStream(int sd) : _sd(sd) {}
      void flush();

      template<class T> inline NetStream& operator<<(const T& v) {_writeBuffer << v; return *this;}
      template<class T> inline NetStream& operator>>(T& v) {_readBuffer >> v; return *this;}
      template<class T> inline NetStream& operator<(const T& v) {_writeBuffer < v; return *this;}
      template<class T> inline NetStream& operator>(T& v) {_readBuffer > v; return *this;}
  };
}

#endif

