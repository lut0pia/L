#pragma once

#include <L/src/L.h>

namespace L {
  class WAV : public Interface<AudioStream> {
    static WAV instance;
  public:
    WAV() : Interface{"wav","wave"} {}

    Ref<AudioStream> from(const byte* data, size_t size) override {
      if(size<44 || memcmp(data, "RIFF", 4) || memcmp(data+8, "WAVE", 4)
         || memcmp(data+12, "fmt ", 4)|| memcmp(data+36, "data", 4))
        return nullptr;
      uint32_t wav_format(read_int(data+20, 2)),
        channels(read_int(data+22, 2)),
        frequency(read_int(data+24)),
        bits_per_sample(read_int(data+34, 2)),
        data_size(read_int(data+40));

      if(wav_format!=1 || data_size > size+44)
        return nullptr;

      Audio::SampleFormat format;
      switch(channels<<8 | bits_per_sample) {
        case 0x108: format = Audio::Mono8; break;
        case 0x110: format = Audio::Mono16; break;
        case 0x208: format = Audio::Stereo8; break;
        case 0x210: format = Audio::Stereo16; break;
        default: return nullptr;
      }

      return ref<AudioBuffer>(data+44, data_size, format, frequency);
    }
    static uint32_t read_int(const byte* data, size_t size = 4) {
      uint32_t wtr(0), offset(0);
      switch(size) {
        case 4: wtr |= uint32_t(*data++) << offset;
          offset += 8;
        case 3: wtr |= uint32_t(*data++) << offset;
          offset += 8;
        case 2: wtr |= uint32_t(*data++) << offset;
          offset += 8;
        case 1: wtr |= uint32_t(*data) << offset;
      }
      return wtr;
    }
  };
  WAV WAV::instance;
}
