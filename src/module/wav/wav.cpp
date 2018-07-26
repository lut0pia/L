#include <L/src/audio/AudioBuffer.h>
#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>

using namespace L;

inline static uint32_t wav_read_int(const uint8_t* data, size_t size = 4) {
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
bool wav_loader(ResourceSlot& slot, AudioStream*& intermediate) {
  Buffer buffer(slot.read_source_file());
  const uint8_t* data((uint8_t*)buffer.data());
  const size_t size(buffer.size());
  if(size<44 || memcmp(data, "RIFF", 4) || memcmp(data+8, "WAVE", 4)
    || memcmp(data+12, "fmt ", 4)|| memcmp(data+36, "data", 4))
    return false;
  uint32_t wav_format(wav_read_int(data+20, 2)),
    channels(wav_read_int(data+22, 2)),
    frequency(wav_read_int(data+24)),
    bits_per_sample(wav_read_int(data+34, 2)),
    data_size(wav_read_int(data+40));

  if(wav_format!=1 || data_size > size+44)
    return false;

  Audio::SampleFormat format;
  switch(channels<<8 | bits_per_sample) {
    case 0x108: format = Audio::Mono8; break;
    case 0x110: format = Audio::Mono16; break;
    case 0x208: format = Audio::Stereo8; break;
    case 0x210: format = Audio::Stereo16; break;
    default: return false;
  }

  intermediate = Memory::new_type<AudioBuffer>(data+44, data_size, format, frequency);
  return true;
}

void wav_module_init() {
  ResourceLoading<AudioStream>::add_loader("wav", wav_loader);
}
