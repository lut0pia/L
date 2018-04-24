#pragma once

#include <L/src/L.h>
#include <stb/stb_vorbis.c>
#undef L // Defined by stb_vorbis.c

namespace L {
  class STB_vorbis : public Interface<AudioStream> {
    static STB_vorbis instance;
    class VorbisStream : public AudioStream {
    protected:
      static const size_t block_size = 1<<14;
      stb_vorbis* _handle;
      stb_vorbis_info _info;
      Array<Buffer> _buffers;
      void* _data;
      size_t _size;
      uint32_t _sample_count, _block_sample_count;
      Audio::SampleFormat _format;
    public:
      VorbisStream(stb_vorbis* handle, void* data, size_t size) : _handle(handle), _data(data), _size(size) {
        _info = stb_vorbis_get_info(handle);
        _sample_count = stb_vorbis_stream_length_in_samples(handle);
        _format = _info.channels==1 ? Audio::Mono16 : Audio::Stereo16;
        _block_sample_count = Audio::convert_samples_required_count(Audio::working_frequency, _info.sample_rate, block_size);
      }
      ~VorbisStream() {
        stb_vorbis_close(_handle);
        Memory::free(_data, _size);
      }
      void render(void* dst, uint32_t dst_sample_start, uint32_t dst_sample_count, float volume[2]) override {
        if(_info.sample_rate==Audio::working_frequency) {
          if(abs(intptr_t(dst_sample_start)-intptr_t(_handle->current_loc))>Audio::working_frequency/2) // More than half a second difference
            stb_vorbis_seek(_handle, dst_sample_start);
          const size_t value_count(dst_sample_count*_info.channels);
          int16_t* buffer(Memory::allocType<int16_t>(value_count));
          dst_sample_count = stb_vorbis_get_samples_short_interleaved(_handle, _info.channels, buffer, value_count);
          Audio::render(dst, buffer, _format, dst_sample_count, volume);
          Memory::freeType(buffer, value_count);
        } else {
          error("Vorbis files with sample rate differing from %d are not supported", Audio::working_frequency);
          /*
          // Compute block index for sample_start
          const uintptr_t start_block(dst_sample_start/_block_sample_count);
          return;
          uint32_t src_sample_start(Audio::convert_samples_reverse_required_count(Audio::working_frequency, _info.sample_rate, dst_sample_start));
          if(abs(intptr_t(src_sample_start)-intptr_t(_handle->current_loc))>Audio::working_frequency/2) // More than half a second difference
            stb_vorbis_seek(_handle, src_sample_start);
          uint32_t src_sample_count(Audio::convert_samples_reverse_required_count(Audio::working_frequency, _info.sample_rate, dst_sample_count));
          const size_t value_count(src_sample_count*_info.channels);
          int16_t* buffer(Memory::allocType<int16_t>(value_count));
          src_sample_count = stb_vorbis_get_samples_short_interleaved(_handle, _info.channels, buffer, value_count);
          Audio::render(dst, buffer, _format, _info.sample_rate, src_sample_count, volume);
          Memory::freeType(buffer, value_count);
          */
        }
      }

      inline uint32_t sample_count() const override { return _sample_count; }
      inline Audio::SampleFormat format() const override { return _format; }
    };
  public:
    STB_vorbis() : Interface{"ogg"} {}

    Ref<AudioStream> from(const uint8_t* data, size_t size) override {
      int error;
      uint8_t* copied_data((uint8_t*)Memory::alloc(size));
      memcpy(copied_data, data, size);
      if(stb_vorbis* handle = stb_vorbis_open_memory(copied_data, size, &error, nullptr))
        return ref<VorbisStream>(handle, copied_data, size);
      else {
        Memory::free(copied_data, size);
        return nullptr;
      }
    }
  };
  STB_vorbis STB_vorbis::instance;
}
