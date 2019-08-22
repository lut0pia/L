#include <L/src/audio/AudioStream.h>
#include <L/src/container/Array.h>
#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/stream/CFileStream.h>

#pragma warning(push, 0)
#include "stb_vorbis.c"
#pragma warning(pop)
#undef L // Defined by stb_vorbis.c

using namespace L;

class VorbisStream : public AudioStream {
protected:
  static const size_t block_size = 1<<14;
  stb_vorbis* _handle;
  stb_vorbis_info _info;
  Buffer _buffer;
  uint32_t _sample_count;
  Audio::SampleFormat _format;
public:
  VorbisStream(stb_vorbis* handle, Buffer& buffer) : _handle(handle), _buffer((Buffer&&)buffer) {
    _info = stb_vorbis_get_info(handle);
    _sample_count = stb_vorbis_stream_length_in_samples(handle);
    _format = _info.channels==1 ? Audio::Mono16 : Audio::Stereo16;
  }
  ~VorbisStream() {
    stb_vorbis_close(_handle);
  }
  void render(void* dst, uint32_t dst_sample_start, uint32_t dst_sample_count, float volume[2]) override {
    if(_info.sample_rate==Audio::working_frequency) {
      if(abs(intptr_t(dst_sample_start)-intptr_t(_handle->current_loc))>Audio::working_frequency/2) // More than half a second difference
        stb_vorbis_seek(_handle, dst_sample_start);
      const size_t value_count(dst_sample_count*_info.channels);
      int16_t* buffer(Memory::alloc_type<int16_t>(value_count));
      dst_sample_count = stb_vorbis_get_samples_short_interleaved(_handle, _info.channels, buffer, int(value_count));
      Audio::render(dst, buffer, _format, dst_sample_count, volume);
      Memory::free_type(buffer, value_count);
    } else {
      error("Vorbis files with sample rate differing from %d are not supported", Audio::working_frequency);
    }
  }

  inline uint32_t sample_count() const override { return _sample_count; }
  inline Audio::SampleFormat format() const override { return _format; }
};

bool stb_vorbis_loader(ResourceSlot& slot, AudioStream*& intermediate) {
  int error;
  Buffer buffer(slot.read_source_file());
  if(stb_vorbis* handle = stb_vorbis_open_memory((uint8_t*)buffer.data(), int(buffer.size()), &error, nullptr)) {
    intermediate = Memory::new_type<VorbisStream>(handle, buffer);
    return true;
  } else {
    return false;
  }
}

void stb_vorbis_module_init() {
  ResourceLoading<AudioStream>::add_loader("ogg", stb_vorbis_loader);
}
