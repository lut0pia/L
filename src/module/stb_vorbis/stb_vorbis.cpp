#include <L/src/audio/AudioDecoder.h>
#include <L/src/audio/AudioStream.h>
#include <L/src/container/Array.h>
#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/macros.h>
#include <L/src/stream/CFileStream.h>

L_PUSH_NO_WARNINGS

#include "stb_vorbis.c"
#undef L // Defined by stb_vorbis.c

L_POP_NO_WARNINGS

using namespace L;

static const Symbol ogg_symbol("ogg"), vorbis_symbol("vorbis");

static bool stb_vorbis_loader(ResourceSlot& slot, AudioStream& intermediate) {
  if(slot.ext != ogg_symbol) {
    return false;
  }

  int error;
  Buffer buffer(slot.read_source_file());
  if(stb_vorbis* handle = stb_vorbis_open_memory((uint8_t*)buffer.data(), int(buffer.size()), &error, nullptr)) {
    stb_vorbis_info info = stb_vorbis_get_info(handle);

    if(info.sample_rate != Audio::working_frequency) {
      warning("Vorbis stream with sample rate differing from %d are not supported", Audio::working_frequency);
      stb_vorbis_close(handle);
      return false;
    }

    intermediate.format = vorbis_symbol;
    intermediate.samples = Buffer(buffer.data(), buffer.size());
    intermediate.sample_count = stb_vorbis_stream_length_in_samples(handle);
    intermediate.sample_format = info.channels == 1 ? Audio::Mono16 : Audio::Stereo16;

    stb_vorbis_close(handle);
    return true;
  }

  return false;
}

class VorbisAudioDecoder : public AudioDecoder {
protected:
  stb_vorbis* _handle = nullptr;
public:
  VorbisAudioDecoder(const Resource<AudioStream>& stream) : AudioDecoder(stream) {}
  ~VorbisAudioDecoder() {
    if(_handle) {
      stb_vorbis_close(_handle);
    }
  }
  void render(void* dst, uint32_t dst_sample_count, float volume[2]) override {
    { // Make sure we have an stb_vorbis handle
      if(!_handle && _stream.is_loaded()) {
        int error;
        const Buffer& samples = _stream->samples;
        _handle = stb_vorbis_open_memory((uint8_t*)samples.data(), int(samples.size()), &error, nullptr);
      }

      if(!_handle) {
        return;
      }
    }

    // Do a seek if more than half a second difference
    if(abs(intptr_t(_current_frame) - intptr_t(_handle->current_loc)) > Audio::working_frequency / 2) {
      stb_vorbis_seek(_handle, _current_frame);
    }

    const Audio::SampleFormat sample_format = _stream->sample_format;
    const uint32_t channel_count = Audio::sample_format_channels(sample_format);
    const size_t value_count = dst_sample_count*channel_count;
    int16_t* buffer(Memory::alloc_type<int16_t>(value_count));
    dst_sample_count = stb_vorbis_get_samples_short_interleaved(_handle, channel_count, buffer, int(value_count));
    Audio::render(dst, buffer, sample_format, dst_sample_count, volume);
    Memory::free_type(buffer, value_count);

    _current_frame += dst_sample_count;
  }
};

static Ref<AudioDecoder> vorbis_audio_decoder_factory(const Resource<AudioStream>& stream) {
  return ref<VorbisAudioDecoder>(stream);
}

void stb_vorbis_module_init() {
  ResourceLoading<AudioStream>::add_loader(stb_vorbis_loader);
  AudioDecoder::register_factory(vorbis_symbol, vorbis_audio_decoder_factory);
}
