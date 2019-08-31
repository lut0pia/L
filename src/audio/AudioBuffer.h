#pragma once

#include "AudioStream.h"
#include "../container/Array.h"

namespace L {
  class AudioBuffer : public AudioStream {
  protected:
    void* _data;
    uint32_t _sample_count;
    Audio::SampleFormat _format;
  public:
    AudioBuffer(const void* data, size_t size, Audio::SampleFormat = Audio::working_format, uint32_t frequency = Audio::working_frequency);
    ~AudioBuffer();
    void render(void* buffer, uint32_t frame_start, uint32_t frame_count, float volume[2]) const override;

    inline uint32_t sample_count() const override { return _sample_count; }
    inline Audio::SampleFormat format() const override { return _format; }
  };
}
