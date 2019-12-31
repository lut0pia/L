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

    friend inline Stream& operator<=(Stream& s, const Intermediate& v) { return s <= v.format <= v.samples <= v.sample_count <= v.sample_format; }
    friend inline Stream& operator>=(Stream& s, Intermediate& v) { return s >= v.format >= v.samples >= v.sample_count >= v.sample_format; }
  };
}
