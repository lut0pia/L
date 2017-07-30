#include "AudioSource.h"

#include "../macros.h"
#include "al.h"

using namespace L;
using namespace Audio;

#define _sid (_data.as<ALuint>())

Source::Source() {
  alGenSources(1, &_sid);
  L_ASSERT(_sid);
}
Source::~Source() {
  alSourcei(_sid, AL_BUFFER, 0);
  alDeleteSources(1, &_sid);
}
void Source::buffer(const Buffer& buffer) {
  alSourcei(_sid, AL_BUFFER, buffer.bid());
}
void Source::play() {
  alSourceRewind(_sid);
  alSourcePlay(_sid);
}
void Source::position(const Vector3f& pos) {
  alSource3f(_sid, AL_POSITION, pos.x(), pos.y(), pos.z());
}
void Source::gain(float g) {
  alSourcef(_sid, AL_GAIN, g);
}
void Source::rolloff(float r) {
  alSourcef(_sid, AL_ROLLOFF_FACTOR, r);
}
void Source::looping(bool should_loop) {
  alSourcei(_sid, AL_LOOPING, should_loop);
}
