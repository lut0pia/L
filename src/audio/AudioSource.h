#pragma once

#include "AudioBuffer.h"
#include "../containers/Raw.h"
#include "../math/Vector.h"

namespace L {
  namespace Audio {
    class Source {
    protected:
      Raw<8> _data;
    public:
      Source();
      ~Source();
      void buffer(const Buffer&);
      void play();
      void stop();
      void position(const Vector3f&);
      void gain(float);
      void rolloff(float);
      void looping(bool);
    };
  }
}
