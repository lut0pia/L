#pragma once

#include <stdint.h>

namespace L {
  class AudioOutput {
  public:
    AudioOutput();
    virtual uint32_t frame_count_ahead() = 0;
    virtual uint32_t frequency() = 0;
    virtual void write(void* buffer, uint32_t frame_count) = 0;

    static AudioOutput* instance();
  };
}
