#include "Audio.h"

#include "../macros.h"
#include "al.h"
#include "alc.h"

using namespace L;

ALCdevice* device(nullptr);
ALCcontext* context(nullptr);

void Audio::init() {
  L_ASSERT(!device);
  device = alcOpenDevice(nullptr);
  L_ASSERT(device);
  context = alcCreateContext(device, nullptr);
  L_ASSERT(context);
  alcMakeContextCurrent(context);
}
void Audio::listener_position(const Vector3f& p) {
  alListener3f(AL_POSITION, p.x(), p.y(), p.z());
}
void Audio::listener_velocity(const Vector3f& v) {
  alListener3f(AL_VELOCITY, v.x(), v.y(), v.z());
}
void Audio::listener_orientation(const Vector3f& f, const Vector3f& u) {
  Vector3f v[2]{f,u};
  alListenerfv(AL_ORIENTATION, (float*)v);
}
