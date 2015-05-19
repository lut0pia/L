#include "Stream.h"

using namespace L;
using namespace Network;

StreamBuf::StreamBuf(SOCKET sd) : sd(sd), buffered(false) {}

std::streamsize StreamBuf::xsgetn(char* s, std::streamsize n) {
  if(buffered) {
    buffered = false;
    *s = buffer;
    s++;
    n--;
  }
  return ::recv(sd,s,n,0);
}
int StreamBuf::underflow() {
  if(buffered)
    return buffer;
  else {
    buffered = true;
    if(::recv(sd,&buffer,1,0) != -1)
      return buffer;
    else
      return EOF;
  }
}
int StreamBuf::uflow() {
  if(buffered) {
    buffered = false;
    return buffer;
  } else {
    char c;
    if(::recv(sd,&c,1,0) != -1)
      return c;
    else
      return EOF;
  }
}

std::streamsize StreamBuf::xsputn(const char* s, std::streamsize n) {
  return ::send(sd,s,n,0);
}
int StreamBuf::overflow(int i) {
  char c(i);
  if(::send(sd,&c,1,0) != -1)
    return c;
  else
    return EOF;
}

Network::Stream::Stream(SOCKET sd) : std::iostream(sb = new StreamBuf(sd)) {}
Network::Stream::~Stream() {
  delete sb;
}
