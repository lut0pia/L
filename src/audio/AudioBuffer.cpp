#include "AudioBuffer.h"

#include "AL/al.h"

using namespace L;
using namespace Audio;

#define _bid (_data.as<ALuint>())

Buffer::Buffer() {
  alGenBuffers(1, &_bid);
}
void Buffer::data(Format format, const void* data, size_t size, size_t frequency) {
  ALenum al_format;
  switch(format) {
    case Mono8: al_format = AL_FORMAT_MONO8; break;
    case Mono16: al_format = AL_FORMAT_MONO16; break;
    case Stereo8: al_format = AL_FORMAT_STEREO8; break;
    case Stereo16: al_format = AL_FORMAT_STEREO16; break;
    default: return;
  }
  alBufferData(_bid, al_format, data, ALsizei(size), ALsizei(frequency));
}
uint32_t Buffer::bid() const {
  return _bid;
}