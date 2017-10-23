#include "Audio.h"

#include "../engine/Engine.h"

using namespace L;

Time Audio::ideal_time_ahead() {
  return Engine::delta_time()*int64_t(2);
}