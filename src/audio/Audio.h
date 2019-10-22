#pragma once

#include <cstddef>

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

    static constexpr uint32_t working_frequency = 44100;
    static constexpr SampleFormat working_format = Stereo16;

    void acquire_buffer(void*& buffer, uint32_t& frame_count);
    void commit_buffer();

    void render(void* dst, const void* src, SampleFormat format, uint32_t sample_count, float volume[2]);
    bool convert_samples(void* dst, SampleFormat dst_fmt, uint32_t dst_freq, const void* src, SampleFormat src_fmt, uint32_t src_freq, uint32_t sample_count);
    uint32_t convert_samples_required_count(uint32_t dst_freq, uint32_t src_freq, uint32_t src_sample_count);
    uint32_t convert_samples_reverse_required_count(uint32_t dst_freq, uint32_t src_freq, uint32_t dst_sample_count);
  }
}
