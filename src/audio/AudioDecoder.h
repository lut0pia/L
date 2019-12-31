#pragma once

#include "AudioStream.h"
#include "../container/Ref.h"
#include "../engine/Resource.h"

namespace L {
  class AudioDecoder {
  protected:
    Resource<AudioStream> _stream;
    uint32_t _current_frame = 0;

  public:
    inline AudioDecoder(const Resource<AudioStream>& stream) : _stream(stream) {}
    virtual ~AudioDecoder() {}

    virtual void render(void* buffer, uint32_t frame_count, float volume[2]) = 0;

    virtual bool playing() const;

    inline const Resource<AudioStream> stream() const { return _stream; }

    typedef Ref<AudioDecoder>(*Factory)(const Resource<AudioStream>&);
    static void register_factory(const Symbol& format, Factory);
    static Ref<AudioDecoder> make(const Resource<AudioStream>&);
  };
}
