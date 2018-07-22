#pragma once

#include "Audio.h"

namespace L {
  class AudioStream {
  public:
    typedef AudioStream* Intermediate;
    virtual ~AudioStream() {}
    virtual void render(void* buffer, uint32_t frame_start, uint32_t frame_count, float volume[2]) = 0;
    virtual uint32_t sample_count() const = 0;
    virtual Audio::SampleFormat format() const = 0;
  };
}
