#include "AudioBuffer.h"

using namespace L;
using namespace Audio;

AudioBuffer::AudioBuffer(const void* data, size_t size, SampleFormat format, uint32_t frequency) {
  const size_t sample_size(sample_format_size(format));
  const uint32_t sample_count(uint32_t(size/sample_size));
  L_ASSERT(sample_size*sample_count==size);
  _format = format;
  _sample_count = convert_samples_required_count(working_frequency, frequency, sample_count);
  _data = Memory::alloc(_sample_count*sample_size);
  convert_samples(_data, _format, working_frequency, data, format, frequency, sample_count);
}
AudioBuffer::~AudioBuffer() {
  Memory::free(_data, _sample_count*sample_format_size(_format));
}

void AudioBuffer::render(void* buffer, uint32_t frame_start, uint32_t frame_count, float volume[2]) const {
  if(frame_start<_sample_count) {
    const uint32_t copy_sample_count(min(frame_count, _sample_count-frame_start));
    Audio::render(buffer, (const char*)_data+frame_start*sample_format_size(_format), _format, copy_sample_count, volume);
  }
}