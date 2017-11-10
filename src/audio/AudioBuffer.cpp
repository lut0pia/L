#include "AudioBuffer.h"

using namespace L;
using namespace Audio;

Buffer::~Buffer() {
  if(_data)
    Memory::free(_data, _sample_count*sample_format_size(_format));
}
void Buffer::data(SampleFormat format, uint32_t frequency, const void* data, size_t size) {
  const size_t sample_size(sample_format_size(format));
  this->~Buffer(); // Free resources
  _format = format;
  if(frequency==working_frequency) {
    _sample_count = size/sample_size;
    _data = Memory::alloc(size);
    memcpy(_data, data, size);
  } else if(frequency==working_frequency*2) { // Can copy every second sample
    _sample_count = size/(sample_size*2);
    _data = Memory::alloc(size/2);
    for(uintptr_t i(0); i<_sample_count; i++)
      memcpy((uint8_t*)_data+sample_size*i, (uint8_t*)data+sample_size*i*2, sample_size);
  } else if(frequency<working_frequency) { // Have to upsample
    const float ratio(float(frequency)/float(working_frequency));
    const uint32_t in_sample_count(size/sample_size);
    const uintptr_t channel_count(sample_format_channels(format));
    _sample_count = in_sample_count / ratio;
    _data = Memory::alloc(_sample_count*sample_size);
    switch(_format) {
      case Mono16:
      {
        int16_t* out_data((int16_t*)_data);
        const int16_t* in_data((const int16_t*)data);

        for(uintptr_t i(0); i<_sample_count; i++) {
          float dummy;
          const float in_index(i*ratio);
          const float fract(modff(in_index, &dummy));
          const uintptr_t in_index_a(in_index);
          const uintptr_t in_index_b(min<uintptr_t>(in_index_a+1, in_sample_count-1));
          const int16_t& in_value_a(in_data[in_index_a]);
          const int16_t& in_value_b(in_data[in_index_b]);
          out_data[i] = in_value_a*(1.f-fract)+in_value_b*fract;
        }
      }
      break;
      case Stereo16:
      {
        int16_t* out_data((int16_t*)_data);
        const int16_t* in_data((const int16_t*)data);

        for(uintptr_t i(0); i<_sample_count; i++) {
          float dummy;
          const float in_index(i*ratio);
          const float fract(modff(in_index, &dummy));
          const uintptr_t in_index_a(in_index);
          const uintptr_t in_index_b(min<uintptr_t>(in_index_a+1, in_sample_count-1));
          const int16_t& in_value_left_a(in_data[2*in_index_a]);
          const int16_t& in_value_left_b(in_data[2*in_index_b]);
          const int16_t& in_value_right_a(in_data[2*in_index_b+1]);
          const int16_t& in_value_right_b(in_data[2*in_index_b+1]);
          out_data[2*i] = in_value_left_a*(1.f-fract)+in_value_left_b*fract;
          out_data[2*i+1] = in_value_right_a*(1.f-fract)+in_value_right_b*fract;
        }
      }
      break;
      default:
        this->~Buffer();
        warning("Unhandled format for resampling");
        break;
    }
  } else warning("Audio downsampling is not unhandled");
}
void Buffer::render(void* buffer, uint32_t frame_start, uint32_t frame_count, float volume[2]) {
  if(frame_start<_sample_count) {
    const uint32_t copy_frame_count(min(frame_count, _sample_count-frame_start));
    if(working_format==_format) {
      int16_t* out_buffer((int16_t*)buffer);
      int16_t* in_data((int16_t*)_data);
      for(uintptr_t i(0); i<copy_frame_count; i++) {
        const int32_t left_mix(int32_t(out_buffer[2*i]) + int32_t(in_data[2*(frame_start+i)])*volume[0]);
        const int32_t right_mix(int32_t(out_buffer[2*i+1]) + int32_t(in_data[2*(frame_start+i)+1])*volume[1]);
        out_buffer[2*i] = clamp<int32_t>(left_mix, INT16_MIN, INT16_MAX);
        out_buffer[2*i+1] = clamp<int32_t>(right_mix, INT16_MIN, INT16_MAX);
      }
    } else if(working_format==Stereo16 && _format==Mono16) {
      int16_t* out_buffer((int16_t*)buffer);
      int16_t* in_data((int16_t*)_data);
      for(uintptr_t i(0); i<copy_frame_count; i++) {
        const int16_t& frame_value(in_data[frame_start+i]);
        const int32_t left_mix(int32_t(out_buffer[2*i]) + frame_value*volume[0]);
        const int32_t right_mix(int32_t(out_buffer[2*i+1]) + frame_value*volume[1]);
        out_buffer[2*i] = clamp<int32_t>(left_mix, INT16_MIN, INT16_MAX);
        out_buffer[2*i+1] = clamp<int32_t>(right_mix, INT16_MIN, INT16_MAX);
      }
    } else warning("Unable to render audio because of unhandled format conversion.");
  }
}
