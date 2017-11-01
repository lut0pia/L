#pragma once

#include "../types.h"
#include "../time/Time.h"

namespace L {
  namespace Audio {
    enum SampleFormat : uint8_t {
      Mono8, Mono16,
      Stereo8, Stereo16,
    };
    static constexpr size_t sample_format_size_array[] = {1,2,2,4};
    constexpr size_t sample_format_size(const SampleFormat& sf) { return sample_format_size_array[sf]; }
    static constexpr uint32_t sample_format_channels_array[] = {1,1,2,2};
    constexpr uint32_t sample_format_channels(const SampleFormat& sf) { return sample_format_channels_array[sf]; }

    static constexpr uint32_t working_frequency = 48000;
    static constexpr SampleFormat working_format = Stereo16;

    void init();
    Time ideal_time_ahead();
    void acquire_buffer(void*& buffer, uint32_t& frame_count);
    void commit_buffer();
  }
}
