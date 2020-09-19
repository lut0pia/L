#include "Audio.h"

#include "../engine/Engine.h"
#include "AudioOutput.h"

using namespace L;
using namespace Audio;

char sample_buffer[sample_format_size(working_format)*working_frequency];
char internal_sample_buffer[sample_format_size(working_format)*working_frequency];
uint32_t requested_frame_count(0);
void Audio::acquire_buffer(void*& buffer, uint32_t& frame_count) {
  L_ASSERT(requested_frame_count==0);

  if(AudioOutput::instance()==nullptr) {
    return;
  }
  AudioOutput& output(*AudioOutput::instance());

  const Time ideal_time_ahead(Engine::delta_time()*int64_t(2));

  // How many frames of advance do we have?
  const uint32_t frame_count_ahead(convert_samples_required_count(working_frequency, output.frequency(), output.frame_count_ahead()));

  // How many frames of advance do we want?
  const uint32_t ideal_frame_count_ahead(uint32_t(ideal_time_ahead.seconds_float()*working_frequency));
  frame_count = requested_frame_count = (frame_count_ahead<ideal_frame_count_ahead) ? (ideal_frame_count_ahead-frame_count_ahead) : 0;

  memset(sample_buffer, 0, frame_count*sample_format_size(working_format));
  buffer = sample_buffer;
}
void Audio::commit_buffer() {
  L_ASSERT(requested_frame_count>0);

  if(AudioOutput::instance()==nullptr) {
    return;
  }
  AudioOutput& output(*AudioOutput::instance());

  const uint32_t internal_freq(output.frequency());
  const uint32_t converted_frame_count(convert_samples_required_count(internal_freq, working_frequency, requested_frame_count));

  convert_samples(internal_sample_buffer, working_format, internal_freq, sample_buffer, working_format, working_frequency, requested_frame_count);

  output.write(internal_sample_buffer, converted_frame_count);

  requested_frame_count = 0;
}

void Audio::render(void* dst, const void* src, SampleFormat format, uint32_t sample_count, float volume[2]) {
  static_assert(working_format == Stereo16, "Unable to render audio because of unhandled format conversion.");
  if(format == Stereo16) {
    int16_t* out_buffer((int16_t*)dst);
    const int16_t* in_data((const int16_t*)src);
    for(uintptr_t i(0); i < sample_count; i++) {
      const int32_t left_mix(int32_t(int32_t(out_buffer[2 * i]) + in_data[2 * (i)] * volume[0]));
      const int32_t right_mix(int32_t(int32_t(out_buffer[2 * i + 1]) + in_data[2 * (i)+1] * volume[1]));
      out_buffer[2 * i] = int16_t(clamp<int32_t>(left_mix, int32_t(INT16_MIN), int32_t(INT16_MAX)));
      out_buffer[2 * i + 1] = int16_t(clamp<int32_t>(right_mix, int32_t(INT16_MIN), int32_t(INT16_MAX)));
    }
    return;
  } else if(format == Mono16) {
    int16_t* out_buffer((int16_t*)dst);
    const int16_t* in_data((const int16_t*)src);
    for(uintptr_t i(0); i < sample_count; i++) {
      const int16_t& frame_value(in_data[i]);
      const int32_t left_mix(int32_t(int32_t(out_buffer[2 * i]) + frame_value*volume[0]));
      const int32_t right_mix(int32_t(int32_t(out_buffer[2 * i + 1]) + frame_value*volume[1]));
      out_buffer[2 * i] = int16_t(clamp<int32_t>(left_mix, int32_t(INT16_MIN), int32_t(INT16_MAX)));
      out_buffer[2 * i + 1] = int16_t(clamp<int32_t>(right_mix, int32_t(INT16_MIN), int32_t(INT16_MAX)));
    }
    return;
  }
  warning("Unable to render audio because of unhandled format conversion.");
}
bool Audio::convert_samples(void* dst, SampleFormat dst_fmt, uint32_t dst_freq, const void* src, SampleFormat src_fmt, uint32_t src_freq, uint32_t sample_count) {
  if(dst_fmt != src_fmt) return false;
  const size_t sample_size(sample_format_size(src_fmt));
  const uint32_t dst_sample_count(convert_samples_required_count(dst_freq, src_freq, sample_count));
  if(dst_freq==src_freq) {
    // No conversion necessary
    memcpy(dst, src, sample_count*sample_size);
    return true;
  } else if(src_freq==dst_freq*2) { // Can copy every second sample
    L_ASSERT(dst_sample_count == sample_count / 2);
    for(uintptr_t i(0); i<dst_sample_count; i++)
      memcpy((uint8_t*)dst+sample_size*i, (const uint8_t*)src+sample_size*i*2, sample_size);
    return true;
  } else if(src_freq<dst_freq) { // Have to upsample
    const float ratio(float(src_freq)/float(dst_freq));
    switch(src_fmt) {
      case Mono16:
      {
        int16_t* out_data((int16_t*)dst);
        const int16_t* in_data((const int16_t*)src);

        for(uintptr_t i(0); i<dst_sample_count; i++) {
          float dummy;
          const float in_index(i*ratio);
          const float fract(modff(in_index, &dummy));
          const uintptr_t in_index_a((uintptr_t)in_index);
          const uintptr_t in_index_b(min<uintptr_t>(in_index_a+1, sample_count-1));
          const int16_t& in_value_a(in_data[in_index_a]);
          const int16_t& in_value_b(in_data[in_index_b]);
          out_data[i] = int16_t(in_value_a*(1.f - fract) + in_value_b*fract);
        }
        return true;
      }
      break;
      case Stereo16:
      {
        int16_t* out_data((int16_t*)dst);
        const int16_t* in_data((const int16_t*)src);

        for(uintptr_t i(0); i<dst_sample_count; i++) {
          float dummy;
          const float in_index(i*ratio);
          const float fract(modff(in_index, &dummy));
          const uintptr_t in_index_a((uintptr_t)in_index);
          const uintptr_t in_index_b(min<uintptr_t>(in_index_a+1, sample_count-1));
          const int16_t& in_value_left_a(in_data[2*in_index_a]);
          const int16_t& in_value_left_b(in_data[2*in_index_b]);
          const int16_t& in_value_right_a(in_data[2*in_index_b+1]);
          const int16_t& in_value_right_b(in_data[2*in_index_b+1]);
          out_data[2*i] = int16_t(in_value_left_a*(1.f-fract))+int16_t(in_value_left_b*fract);
          out_data[2*i+1] = int16_t(in_value_right_a*(1.f-fract))+int16_t(in_value_right_b*fract);
        }
        return true;
      }
      break;
      default: warning("Unhandled format for resampling"); break;
    }
  } else { // Have to downsample
    const float ratio(float(src_freq)/float(dst_freq));
    switch(src_fmt) {
      case Mono16:
      {
        int16_t* out_data((int16_t*)dst);
        const int16_t* in_data((const int16_t*)src);
        for(uintptr_t i(0); i<dst_sample_count; i++) {
          out_data[i] = in_data[uintptr_t(i*ratio)];
        }
        return true;
      }
      break;
      case Stereo16:
      {
        int16_t* out_data((int16_t*)dst);
        const int16_t* in_data((const int16_t*)src);

        for(uintptr_t i(0); i<dst_sample_count; i++) {
          const uintptr_t in_index((uintptr_t)(i*ratio));
          out_data[2*i] = in_data[2*in_index];
          out_data[2*i+1] = in_data[2*in_index+1];
        }
        return true;
      }
      break;
      default: warning("Unhandled format for resampling"); break;
    }
  }
  return false;
}
uint32_t Audio::convert_samples_required_count(uint32_t dst_freq, uint32_t src_freq, uint32_t src_sample_count) {
  const float ratio(float(src_freq)/float(dst_freq));
  const uint32_t dst_sample_count(uint32_t(src_sample_count / ratio));
  return dst_sample_count;
}
uint32_t Audio::convert_samples_reverse_required_count(uint32_t dst_freq, uint32_t src_freq, uint32_t dst_sample_count) {
  const float ratio(float(src_freq)/float(dst_freq));
  const uint32_t src_sample_count(uint32_t(dst_sample_count * ratio));
  return src_sample_count;
}
