#pragma once

#include "Audio.h"
#include "../container/Array.h"

namespace L {
  namespace Audio {
    class Buffer {
    protected:
      void* _data;
      uint32_t _sample_count;
      SampleFormat _format;
    public:
      constexpr Buffer() : _data(nullptr), _sample_count(0), _format(working_format) {}
      ~Buffer();
      void data(SampleFormat, uint32_t frequency, const void* data, size_t size);
      void render(void* buffer, uint32_t frame_start, uint32_t frame_count, float volume[2]);

      inline uint32_t sample_count() const { return _sample_count; }
      inline SampleFormat format() const { return _format; }
    };
  }
}
