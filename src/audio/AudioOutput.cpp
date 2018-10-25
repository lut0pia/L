#include "AudioOutput.h"

using namespace L;

static AudioOutput* ao_instance(nullptr);

AudioOutput::AudioOutput() {
  ao_instance = this;
}

AudioOutput* AudioOutput::instance() {
  return ao_instance;
}