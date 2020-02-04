#include <L/src/audio/AudioStream.h>
#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>

using namespace L;

static const Symbol wav_symbol("wav");

inline static uint32_t wav_read_int(const uint8_t* data, size_t size = 4) {
  uint32_t wtr(0), offset(0);
  switch(size) {
    case 4: wtr |= uint32_t(*data++) << offset;
      offset += 8;
      // Fallthrough
    case 3: wtr |= uint32_t(*data++) << offset;
      offset += 8;
      // Fallthrough
    case 2: wtr |= uint32_t(*data++) << offset;
      offset += 8;
      // Fallthrough
    case 1: wtr |= uint32_t(*data) << offset;
  }
  return wtr;
}
bool wav_loader(ResourceSlot& slot, AudioStream& intermediate) {
  if(slot.ext != wav_symbol) {
    return false;
  }

  Buffer buffer(slot.read_source_file());
  const uint8_t* data((uint8_t*)buffer.data());
  const size_t size(buffer.size());
  if(size < 44 || memcmp(data, "RIFF", 4) || memcmp(data + 8, "WAVE", 4)
    || memcmp(data + 12, "fmt ", 4) || memcmp(data + 36, "data", 4))
    return false;
  uint32_t wav_format(wav_read_int(data + 20, 2)),
    channels(wav_read_int(data + 22, 2)),
    frequency(wav_read_int(data + 24)),
    bits_per_sample(wav_read_int(data + 34, 2)),
    data_size(wav_read_int(data + 40));

  if(wav_format != 1 || data_size > size + 44)
    return false;

  Audio::SampleFormat format;
  switch(channels << 8 | bits_per_sample) {
    case 0x108: format = Audio::Mono8; break;
    case 0x110: format = Audio::Mono16; break;
    case 0x208: format = Audio::Stereo8; break;
    case 0x210: format = Audio::Stereo16; break;
    default: return false;
  }

  intermediate.format = "raw";

  const size_t sample_size(Audio::sample_format_size(format));
  const uint32_t sample_count(uint32_t(data_size / sample_size));
  L_ASSERT(sample_size*sample_count == data_size);
  intermediate.sample_format = format;
  intermediate.sample_count = Audio::convert_samples_required_count(Audio::working_frequency, frequency, sample_count);
  intermediate.samples = Buffer(sample_count*sample_size);
  convert_samples(intermediate.samples.data(), format, Audio::working_frequency, data, format, frequency, sample_count);
  return true;
}

void wav_module_init() {
  ResourceLoading<AudioStream>::add_loader(wav_loader);
}
