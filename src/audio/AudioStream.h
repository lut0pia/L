#pragma once

#include "Audio.h"
#include "../container/Buffer.h"
#include "../text/Symbol.h"

namespace L {
  struct AudioStream {
    typedef AudioStream Intermediate;

    Symbol format;
    Buffer samples;
    uint32_t sample_count;
    Audio::SampleFormat sample_format;

    friend inline void resource_write(Stream& s, const Intermediate& v) { s <= v.format <= v.samples <= v.sample_count <= v.sample_format; }
    friend inline void resource_read(Stream& s, Intermediate& v) { s >= v.format >= v.samples >= v.sample_count >= v.sample_format; }
  };
}
