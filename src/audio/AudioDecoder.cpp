#include "AudioDecoder.h"

#include "../container/Table.h"

using namespace L;

static Table<Symbol, AudioDecoder::Factory> factories;
static const Symbol raw_symbol = "raw";

class RawAudioDecoder : public AudioDecoder {
public:
  RawAudioDecoder(const Resource<AudioStream>& stream) : AudioDecoder(stream) {}
  void render(void* buffer, uint32_t frame_count, float volume[2]) override {
    const AudioStream* stream = _stream.force_load();
    if(!stream) {
      return;
    }
    const uint32_t sample_count = stream->sample_count;
    if(_current_frame < sample_count) {
      const char* samples = (const char*)stream->samples.data();
      const Audio::SampleFormat sample_format = stream->sample_format;
      const size_t sample_format_size = Audio::sample_format_size(sample_format);
      const uint32_t copy_sample_count(min<uint32_t>(frame_count, sample_count - _current_frame));
      Audio::render(buffer, samples + _current_frame*sample_format_size, sample_format, copy_sample_count, volume);
      _current_frame += copy_sample_count;
    }
  }
};

bool AudioDecoder::playing() const {
  const AudioStream* stream = _stream.force_load();
  return stream && _current_frame < stream->sample_count;
}

void AudioDecoder::register_factory(const Symbol& format, Factory factory) {
  factories[format] = factory;
}
Ref<AudioDecoder> AudioDecoder::make(const Resource<AudioStream>& stream_res) {
  if(const AudioStream* stream = stream_res.force_load()) {
    const Symbol& format = stream->format;
    if(format == raw_symbol) {
      return ref<RawAudioDecoder>(stream_res);
    }
    if(Factory* factory = factories.find(format)) {
      return (*factory)(stream_res);
    }
    warning("Unable to find audio decoder factory for format: %s", format);
  }
  return nullptr;
}
